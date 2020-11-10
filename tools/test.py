import serial

try:
    com = serial.Serial("/dev/ttyS4", baudrate=921600, timeout=1)
except serial.SerialException as e:
    print(e)
    exit()

com.write(b"Hei dette er en liten test\n")