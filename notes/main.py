
import sys
import time

from serial import Serial
from serial.tools import list_ports

data_string = "HELLORLD!"
connected: bool = False

def establish_contact():
    global connected
    # while not port.in_waiting:
    #     pass
    # in_char = port.read()
    # if in_char == b'A':
    port.write(b'A')
    while not port.in_waiting:
        pass
    in_char = port.read()
    if in_char == b'E':
        connected = True
        print("Contact established!")

port = Serial()
port.port = '/dev/cu.usbmodem143301'
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

print('Sending string')

byte_array = bytearray(data_string, 'utf-8')

for char in byte_array:
    # print(char)
    char_byte = bytes([char])
    port.write(char_byte)
    port.flush()  # Ensure data is sent immediately
    
    time.sleep(0.05)  # Give Arduino time to echo back
    
    if port.in_waiting > 0:
        in_char = port.read()
        print(f'{in_char.decode("utf-8")}', end='')  # Print received character without newline
    else:
        print(f'Sent: {char_byte}, Received: No data (timeout)')

quit()
