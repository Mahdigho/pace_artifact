from time import sleep, time

import numpy as np

from analog3 import DWF
from rehostat import prep_itsy, set_wiper

if __name__ == "__main__":
    analog3 = DWF()
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
        in_vals = input("Listening for voltage wiper: ").split()
        voltage = float(in_vals[0])
        wiper = int(in_vals[1])
        analog3.master_switch(True)
        analog3.positive_switch(True)
        analog3.positive_supply(0)
        sleep(0.4)
        print(set_wiper(itsy, wiper))
        analog3.positive_supply(voltage)
        sleep(0.4)
        np.random.seed(42)
        start = time()
        for _ in range(100):
            set_wiper(itsy, wiper)

        print((time() - start))
