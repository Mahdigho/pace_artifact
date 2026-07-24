import select
import sys
from time import sleep, time
from typing import Dict

import numpy

from analog3 import DWF
from rehostat import prep_itsy, set_wiper
from traces import PowerSetting, PowerTrace, constant_traces, interrupted_traces


def has_no_input() -> bool:
    ready = select.select([sys.stdin], [], [], 0)[0]
    return not ready


def play_samples(base: int, samples: numpy.ndarray):
    last_sample = base
    # all traces begin with a one second delay to let the clock begin
    sleep(1)
    # sleep(15)
    while True:
        for sample in samples:
            # set_wiper(itsy, base)
            if last_sample != sample:
                set_wiper(itsy, sample)
                last_sample = sample
            else:
                sleep(0.00835)
                if not has_no_input():
                    return


def play_trace(trace: PowerTrace):
    print("----------------------------------")

    _ = input(f"{round(time(), 3)} Press enter to play trace {trace.name}")
    # print(f"{round(time(), 3)} Press to play trace {trace.name}")
    # clear out the cap
    set_wiper(itsy, 0)
    analog3.positive_supply(0)
    sleep(3.000)

    print(f"Press enter key to stop {trace.name}")
    # start trace thread and set voltage
    analog3.positive_supply(trace.setting.voltage)
    set_wiper(itsy, trace.setting.wiper)
    # wait for it to be over
    play_samples(trace.setting.wiper, trace.samples)

    # stop thread, set supply to zero and stop wiper.
    analog3.positive_supply(0)
    set_wiper(itsy, 0)
    # read all the needed lines
    while not has_no_input():
        _ = sys.stdin.readline()


# The traces used in the paper
EG1 = [18, 24, 29, 34, 40]
EG2 = [48, 58, 85, 110, 140]
EG1_2 = [
    18,
    24,
    29,
    34,
    40,
    48,
    58,
    85,
    110,
    140,
]


def select_traces(trace_package: Dict[int, PowerTrace], count: int, seed: int):
    # All the reasonably spaced wiper vals in case you want to sweep over all of them
    # wiper_vals = [
    #     14,
    #     15,
    #     16,
    #     18,
    #     20,
    #     22,
    #     24,
    #     26,
    #     28,
    #     29,
    #     30,
    #     32,
    #     34,
    #     36,
    #     38,
    #     40,
    #     42,
    #     44,
    #     46,
    #     48,
    #     50,
    #     52,
    #     54,
    #     56,
    #     58,
    #     60,
    #     65,
    #     70,
    #     75,
    #     80,
    #     85,
    #     90,
    #     95,
    #     100,
    #     105,
    #     110,
    #     115,
    #     120,
    #     125,
    #     130,
    #     135,
    #     140,
    #     145,
    #     150,
    #     155,
    #     160,
    # ]

    # Set this when running the main experiment
    wiper_vals = EG1_2
    for wiper in wiper_vals:
        play_trace(trace_package[wiper])


if __name__ == "__main__":
    analog3 = DWF()
    analog3.master_switch(True)
    analog3.positive_switch(True)
    analog3.positive_supply(0)
    print("Analog Device Found?", analog3.device.name.value)

    port_name = input("Provide the itsy tty port:")
    print("Attempting connection at port")
    itsy = prep_itsy(port_name)
    print("Itsy found")

    read = itsy.read_until()
    while len(read) != 0:
        read = itsy.read_until()

    while True:
        print("------------------------")
        mode = input("Enter Mode [volatile/constant]:  ")
        mode = mode.strip()
        trace_package: list[PowerSetting]
        match mode:
            case "constant":
                select_traces(constant_traces, 10, 129)
                # for trace in constant_traces:
                #     play_trace(trace)
            case "volatile":
                select_traces(interrupted_traces, 10, 129)
            case _:
                print("Invalid name, try again")
