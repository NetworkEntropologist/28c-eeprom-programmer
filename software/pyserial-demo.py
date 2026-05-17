

import sys
import time

from serial import Serial
from serial.tools import list_ports

port = Serial()
port.port = '/dev/cu.usbserial-14310'
port.baudrate = 115200
port.bytesize = 8           # 8 data bits (standard)
port.parity = 'N'           # No parity
port.stopbits = 1           # 1 stop bit
port.timeout = 1
port.dtr = False            # Prevents Arduino from resetting when opening the port
port.rts = False            # Disable RTS


print('Opening port')
port.open()

print('Port opened')
while not port.is_open:
    pass

# Give the Arduino time to initialize after opening the port
time.sleep(2)

char_byte = bytes('D', 'utf-8')
port.write(char_byte)
port.flush()  # Ensure data is sent immediately

time.sleep(0.05)  # Give Arduino time to echo back

while True:

    if port.in_waiting > 0:
        in_char = port.read()
        print(f'{in_char}')
        # print(f'{in_char.decode("utf-8")}')  # Print received character without newline

