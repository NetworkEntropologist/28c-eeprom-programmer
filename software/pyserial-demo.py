
import serial
import time

ser = serial.Serial('/dev/cu.usbmodem141301', 115200)

while not ser.in_waiting:
    pass

while True:

    ser.write(b'A')
    while not ser.in_waiting:
        pass
    return_line = ser.readline().decode('utf-8').rstrip()
    print(return_line)

    # ser.write(b'Hellorld!')
    # while not ser.in_waiting:
    #     pass
    # return_line = ser.readline().decode('utf-8').rstrip()
    # print(return_line)



