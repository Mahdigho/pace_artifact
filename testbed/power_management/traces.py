from typing import Dict, Optional, Tuple

import numpy


class PowerSetting:
    def __init__(
        self, voltage: float, wiper: int, resistance: float, name: Optional[str] = None
    ) -> None:
        """Voltage in volts and resistance in ohms"""
        self.wiper = wiper
        self.voltage = voltage
        self.resistance = resistance
        self.max_current = self.voltage / self.resistance
        if name is None:
            self.name = f"{voltage}-{wiper}"
        else:
            self.name = name


class PowerTrace:
    def __init__(
        self,
        name: str,
        setting: PowerSetting,
        samples: numpy.ndarray,
    ):
        self.setting = setting
        self.name = name
        self.samples = samples

    @staticmethod
    def generate_samples(
        sample_length: int,
        base_wiper: int,
        interrupt_count: int,
        interrupt_length: Tuple[int, int],
        interrupt_range: Tuple[float, float],
        interrupt_varability: int,
    ) -> numpy.ndarray:
        # Samples are unaltered unless a interruption is occuring
        samples = numpy.full(sample_length, base_wiper)

        lengths = numpy.random.randint(
            low=interrupt_length[0], high=interrupt_length[1], size=interrupt_count
        )
        interrupt_centers = numpy.random.uniform(
            low=min(200, int(base_wiper * interrupt_range[0])),
            high=min(200, int(base_wiper * interrupt_range[1])),
            size=interrupt_count,
        )
        interrupt_start_idx = numpy.random.randint(
            low=0, high=sample_length - interrupt_length[1], size=interrupt_count
        )

        for length, center, start_idx in zip(
            lengths, interrupt_centers, interrupt_start_idx
        ):
            # print(length, center, start_idx)
            interrupted_samples = numpy.random.uniform(
                low=center - interrupt_varability,
                high=center + interrupt_varability,
                size=length,
            )
            samples[start_idx : start_idx + length] = interrupted_samples

        return samples


base_stats: Dict[int, PowerSetting] = {
    20: PowerSetting(3.2, 10, 855.0 / 2),
    70: PowerSetting(3.2, 14, 855.0 / 2),
    73: PowerSetting(3.2, 15, 855.0 / 2),
    77: PowerSetting(3.2, 16, 929.0 / 2),
    85: PowerSetting(3.2, 18, 1003.0 / 2),
    93: PowerSetting(3.2, 20, 855.0 / 2),
    102: PowerSetting(3.2, 22, 929.0 / 2),
    110: PowerSetting(3.2, 24, 1003.0 / 2),
    119: PowerSetting(3.2, 26, 1076.0 / 2),  # boundary
    129: PowerSetting(3.2, 28, 1150 / 2),
    130: PowerSetting(3.2, 29, 1150 / 2),
    135: PowerSetting(3.2, 30, 1224.0 / 2),  # boundary
    146: PowerSetting(3.2, 32, 1298.0 / 2),
    156: PowerSetting(3.2, 34, 1372.0 / 2),
    163: PowerSetting(3.2, 36, 1446.0 / 2),  # boundary
    172: PowerSetting(3.2, 38, 1520.0 / 2),
    182: PowerSetting(3.2, 40, 1594.0 / 2),
    189: PowerSetting(3.2, 42, 1668.0 / 2),  # boundary
    201: PowerSetting(3.2, 44, 1742.0 / 2),
    210: PowerSetting(3.2, 46, 1816.0 / 2),
    220: PowerSetting(3.2, 48, 1890.0 / 2),
    230: PowerSetting(3.2, 50, 1960.0 / 2),  # boundary
    239: PowerSetting(3.2, 52, 2038.0 / 2),
    249: PowerSetting(3.2, 54, 2111.0 / 2),
    256: PowerSetting(3.2, 56, 2185.0 / 2),
    268: PowerSetting(3.2, 58, 2260.0 / 2),
    279: PowerSetting(3.2, 60, 2333.0 / 2),
    300: PowerSetting(3.2, 65, 2518.0 / 2),
    327: PowerSetting(3.2, 70, 2703.0 / 2),
    354: PowerSetting(3.2, 75, 2888.0 / 2),  # boundary
    382: PowerSetting(3.2, 80, 3072.0 / 2),
    413: PowerSetting(3.2, 85, 3257.0 / 2),
    440: PowerSetting(3.2, 90, 3442.0 / 2),
    468: PowerSetting(3.2, 95, 3627.0 / 2),  # boundary
    500: PowerSetting(3.2, 100, 3812.0 / 2),
    525: PowerSetting(3.2, 105, 3996.0 / 2),
    560: PowerSetting(3.2, 110, 4181.0 / 2),
    586: PowerSetting(3.2, 115, 4366.0 / 2),
    620: PowerSetting(3.2, 120, 4552.0 / 2),
    655: PowerSetting(3.2, 125, 4736.0 / 2),  # boundary
    690: PowerSetting(3.2, 130, 4920.0 / 2),
    720: PowerSetting(3.2, 135, 5105.0 / 2),
    755: PowerSetting(3.2, 140, 5290.0 / 2),
    796: PowerSetting(3.2, 145, 5475.0 / 2),
    830: PowerSetting(3.2, 150, 5600.0 / 2),
    868: PowerSetting(3.2, 155, 5844.0 / 2),
    910: PowerSetting(3.2, 160, 5844.0 / 2),
}

constant_traces = {
    power_setting.wiper: PowerTrace(
        name=f"ConstTrace_{power_setting.wiper}",
        setting=power_setting,
        samples=numpy.array([power_setting.wiper]),
    )
    for power_setting in base_stats.values()
}

numpy.random.seed(42)

interrupted_traces = {
    power_setting.wiper: PowerTrace(
        name=f"InterruptedTrace_{power_setting.wiper}",
        setting=power_setting,
        samples=PowerTrace.generate_samples(
            10000, power_setting.wiper, 50, (40, 60), (2, 3), 3
        ),
    )
    for power_setting in base_stats.values()
}
