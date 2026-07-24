import supervisor
import reho

print("listening...")
rehostat = reho.RheoStat()

# Learned this from https://stackoverflow.com/questions/48922189/receive-data-from-host-computer-using-circuit-python-on-circuit-playground-expre
while True:
    if supervisor.runtime.serial_bytes_available:
        inital_input = input()
        value: str = inital_input.strip()
        # Sometimes Windows sends an extra (or missing) newline - ignore them
        if value == "":
            continue
        if not value.isdigit():
            print(inital_input)
            print("Please Send Integer Ohm Numbers")
        else:
            # rehostat.set_res(int(value), True)
            rehostat._set_wiper(int(value), False)
            print(value)
