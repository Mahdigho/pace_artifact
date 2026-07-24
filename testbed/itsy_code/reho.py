import board
import busio
import digitalio
from adafruit_bus_device.spi_device import SPIDevice


class RheoStat:
    CMD_WRIT = 0b00000000
    CMD_READ = 0b00001100
    ADR_WIP0 = 0b00000000
    ADR_WIP1 = 0b00010000

    # experimentally
    MCP10k = [
        (0, 118),
        (1, 154),
        (2, 192),
        (3, 228),
        (4, 264),
        (5, 302),
        (6, 338),
        (7, 374),
        (8, 412),
        (9, 449),
        (10, 485),
        (20, 857),
        (30, 1226),
        (40, 1598),
        (50, 1965),
        (100, 3800),
        (130, 4920),
        (160, 6030),
        (190, 7140),
        (220, 8250),
        (250, 9360),
        (256, 9580),
    ]
    MCP10k_m = 36.96
    MCP10k_b = 116

    def __init__(self) -> None:
        self.spi = busio.SPI(board.SCK, board.MOSI, board.MISO)
        self.cs = digitalio.DigitalInOut(board.D13)
        self.device = SPIDevice(
            self.spi, self.cs, baudrate=5000000, polarity=0, phase=0
        )
        self.m = self.MCP10k_m
        self.b = self.MCP10k_b

    def ohm2wiper(self, ohms: int) -> int:
        return min(max(0, int((ohms - self.b) / self.m + 0.5)), 256)

    def wiper2ohm(self, wiper: int) -> float:
        return self.b + wiper * self.m

    def _write(self, cmd: bytearray, debug: bool = False):
        write_value = cmd[0] | self.CMD_READ
        cmd[0] |= self.CMD_WRIT
        result = bytearray(2)
        with self.device:
            self.spi.write(cmd)
            self.spi.readinto(result, write_value=write_value)
        if debug:
            print(cmd)
            print(result)

    def _read(self, cmd: int) -> bytearray:
        result = bytearray(2)
        with self.device:
            self.spi.readinto(result, write_value=cmd | self.CMD_READ)
        return result

    def _set_wiper(self, wiper, debug: bool = False):
        to_send = bytearray([0x00, 0x0])
        wiper = max(min(wiper, 256), 0)
        if wiper == 256:
            to_send[0] |= 1
        else:
            to_send[1] = wiper

        if debug:
            print(
                "Wiper is set to ",
                wiper,
                " which through the math is",
                self.wiper2ohm(wiper),
                " ohms",
            )
        # wiper 0
        to_send[0] |= self.ADR_WIP0
        if debug:
            print("wiper 0")
        self._write(to_send, debug)
        # wiper 1
        if debug:
            print("wiper 1")
        to_send[0] |= self.ADR_WIP1
        self._write(to_send, debug)

    def set_res(self, resistance: int, debug: bool = False):
        self._set_wiper(self.ohm2wiper(resistance), debug)
