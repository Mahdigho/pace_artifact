import serial


def prep_itsy(port: str) -> serial.Serial:
    return serial.Serial(port=port, baudrate=115200, timeout=0.005)


def set_wiper(itsy: serial.Serial, wiper: int) -> str:
    ret = ""
    itsy.write(f"{wiper}\n\r".encode())
    read = itsy.read_until()
    while len(read) != 0:
        ret += str(read)
        read = itsy.read_until()

    return ret
