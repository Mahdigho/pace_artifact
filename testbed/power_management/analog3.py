import atexit
import ctypes  # import the C compatible data types
from ctypes import Array, byref, c_bool, c_double, c_int, create_string_buffer
from dataclasses import dataclass
from sys import platform  # this is needed to check the OS type and get the PATH
from typing import List, Tuple

import numpy as np
from numpy import ndarray

import dwfconstants as dfc


@dataclass
class DevData:
    """stores the device handle and the device name"""

    handle: c_int = c_int(0)
    name: ctypes.Array[ctypes.c_char] = create_string_buffer(64)


@dataclass
class CustomWaveGen:
    """
    Dataclass to store custom wavegen information, i.e. our traces.
    Requires sample values in the [-1, 1] range and produces
    voltage values with the below formula

        Voltage: Offset + Sample*Amplitude

    The whole sequence repeats based on the set frequency, regardless
    of how many sample values there are. Frequency should be 1/duration set.

    Channel: 1 or 2 or 0 for both
    Offset: the base voltage
    Amplitude: the range of change

    """

    name: str
    channel: int
    samples: Array[c_double]
    sample_count: int
    duration: float = 2.0
    offset: float = 0
    amplitude: float = 1
    # frequency: c_double = c_double(1.0)

    @staticmethod
    def pylist_to_cdouble(samples: List[float] | ndarray) -> Array[c_double]:
        buffer = (ctypes.c_double * len(samples))()
        for index, sample in enumerate(samples):
            buffer[index] = ctypes.c_double(sample)
        return buffer

    @classmethod
    def gen_interrupt_trace(
        cls,
        sample_length: int,
        interrupt_count: int,
        interrupt_length: Tuple[int, int],
        interrupt_range: Tuple[float, float],
        interrupt_varability: float,
    ) -> Array[c_double]:
        # Samples are unaltered unless a interruption is occuring
        samples = np.zeros(sample_length)

        # create the interrupted regions
        lengths = np.random.randint(
            low=interrupt_length[0], high=interrupt_length[1], size=interrupt_count
        )
        interrupt_centers = np.random.uniform(
            low=interrupt_range[0], high=interrupt_range[1], size=interrupt_count
        )
        interrupt_start_idx = np.random.randint(
            low=0, high=sample_length - interrupt_length[1], size=interrupt_count
        )

        for length, center, start_idx in zip(
            lengths, interrupt_centers, interrupt_start_idx
        ):
            # print(length, center, start_idx)
            interrupted_samples = np.random.uniform(
                low=center - interrupt_varability,
                high=center + interrupt_varability,
                size=length,
            )
            samples[start_idx : start_idx + length] = interrupted_samples

        # print(samples)
        return cls.pylist_to_cdouble(samples)

    def pysamples(self) -> List[float]:
        return [self.samples[i] for i in range(self.sample_count)]


class DWF:
    def __init__(self) -> None:
        self.dwf: ctypes.CDLL
        # load the dynamic library, get constants path (the path is OS specific)

        if platform.startswith("win"):
            self.dwf = ctypes.cdll.dwf
        elif platform.startswith("darwin"):
            self.dwf = ctypes.cdll.LoadLibrary("/Library/Frameworks/dwf.framework/dwf")
        else:
            self.dwf = ctypes.cdll.LoadLibrary("libdwf.so")

        # check library loading errors
        szerr = ctypes.create_string_buffer(512)
        self.dwf.FDwfGetLastErrorMsg(szerr)
        if szerr[0] != b"\0":
            print(str(szerr.value))

        # # check library loading errors

        self.device = self._open(-1)
        atexit.register(self.close)

    def _open(self, number: int) -> DevData:
        """open the first available device"""
        device = DevData()

        # setup enum to get name
        cDevice = c_int()
        self.dwf.FDwfEnum(dfc.devidDiscovery3, byref(cDevice))
        if cDevice.value == 0:
            print("no device found, exiting")
            exit(1)
        # connect to the first available device
        self.dwf.FDwfEnumDeviceName(c_int(0), device.name)

        self.dwf.FDwfDeviceOpen(c_int(0), byref(device.handle))
        if device.handle.value == 0:
            szerr = create_string_buffer(512)
            self.dwf.FDwfGetLastErrorMsg(szerr)
            print(szerr.value)
            exit(1)
        return device

    def master_switch(self, enabled: bool):
        self.dwf.FDwfAnalogIOEnableSet(self.device.handle, c_int(enabled))

    def positive_switch(self, enabled: bool):
        self.dwf.FDwfAnalogIOChannelNodeSet(
            self.device.handle,
            c_int(0),
            c_int(0),
            c_double(enabled),
        )

    def positive_supply(self, voltage: float):
        positive_voltage = max(0, min(5, voltage))
        self.dwf.FDwfAnalogIOChannelNodeSet(
            self.device.handle,
            c_int(0),
            c_int(1),
            c_double(positive_voltage),
        )

    def get_positive_supply(self) -> float:
        self.dwf.FDwfAnalogIOStatus(self.device.handle)
        vpp = c_double()
        self.dwf.FDwfAnalogIOChannelNodeStatus(
            self.device.handle, c_int(0), c_int(1), byref(vpp)
        )
        return vpp.value

    def close(self):
        self.dwf.FDwfDeviceClose(self.device.handle)

    def constant_wavegen(self, voltage: float, channel: int):
        # NOTE: why not use the DC? Cause it turns out the
        # using a custom version with 0 amplitude is more reliable
        # than the DC option :| . Yeah I had the same face.
        self.custom_wavegen(
            CustomWaveGen(
                "DC_Voltage",
                channel,
                CustomWaveGen.pylist_to_cdouble([1]),
                1,
                1,
                voltage,
                0,
            )
        )

    def custom_wavegen(self, custom: CustomWaveGen):
        # enable
        c_channel = c_int(custom.channel - 1)
        self.dwf.FDwfAnalogOutNodeEnableSet(
            self.device.handle, c_channel, dfc.AnalogOutNodeCarrier, c_bool(True)
        )

        # set to Custom
        self.dwf.FDwfAnalogOutNodeFunctionSet(
            self.device.handle, c_channel, dfc.AnalogOutNodeCarrier, dfc.funcCustom
        )

        # load data
        self.dwf.FDwfAnalogOutNodeDataSet(
            self.device.handle,
            c_channel,
            dfc.AnalogOutNodeCarrier,
            custom.samples,
            ctypes.c_int(custom.sample_count),
        )

        # set offset
        self.dwf.FDwfAnalogOutNodeOffsetSet(
            self.device.handle,
            c_channel,
            dfc.AnalogOutNodeCarrier,
            c_double(custom.offset),
        )

        # set amplitude or DC voltage
        self.dwf.FDwfAnalogOutNodeAmplitudeSet(
            self.device.handle,
            c_channel,
            dfc.AnalogOutNodeCarrier,
            ctypes.c_double(custom.amplitude),
        )

        # set frequency
        frequency = 1.0 / custom.duration
        self.dwf.FDwfAnalogOutNodeFrequencySet(
            self.device.handle,
            c_channel,
            dfc.AnalogOutNodeCarrier,
            ctypes.c_double(frequency),
        )

        # start
        self.dwf.FDwfAnalogOutConfigure(self.device.handle, c_channel, c_bool(True))

    def stop_wavegen(self, channel: int):
        channel_c = c_int(channel - 1)
        self.dwf.FDwfAnalogOutReset(self.device.handle, channel_c)
        pass

    def mesaure(self, channel: int) -> float:
        # set up the instrument
        self.dwf.FDwfAnalogInConfigure(self.device.handle, c_bool(False), c_bool(False))
        # read data to an internal buffer
        self.dwf.FDwfAnalogInStatus(self.device.handle, c_bool(False), c_int(0))

        # extract data from that buffer
        voltage = c_double()  # variable to store the measured voltage
        self.dwf.FDwfAnalogInStatusSample(
            self.device.handle, c_int(channel - 1), ctypes.byref(voltage)
        )

        # store the result as float
        voltage = voltage.value
        return voltage
