
import sys
import time

from struct import pack

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

def build_read_command(address: int) -> bytes:

    # Command format: Big Endian, start byte, length, command byte, address (2 bytes)
    format_string = '>BBBH'

    cmd_byte = 'R'.encode('utf-8')[0]
    return pack(format_string, 0x01, 0x03, cmd_byte, address)    

def build_write_command(address: int, data: int) -> bytes:
    
    # Command format: Big Endian, start byte, length, command byte, address (2 bytes), data byte
    format_string = '>BBBHB'

    cmd_byte = 'W'.encode('utf-8')[0]
    return pack(format_string, 0x01, 0x04, cmd_byte, address, data)

while True:
    
    command = input('Enter command (R=read, W=write, Q=quit): ').strip().upper()

    cmd = bytearray()

    if command == 'Q':
        print('Exiting...')
        port.close()
        sys.exit(0)
    elif command == 'W':
        address = int(input('Enter address to write (hex): '), 16)
        data = int(input('Enter data to write (hex): '), 16)
        cmd = build_write_command(address=address, data=data)
        
    elif command == 'R':
        address = int(input('Enter address to read (hex): '), 16)
        cmd = build_read_command(address=address)
        
    print('Sending command')

    port.write(cmd)
    port.flush()  # Ensure data is sent immediately

    time.sleep(0.5)  # Give Arduino time to process the command

    while not port.in_waiting:
        pass

    if port.in_waiting > 0:
        print('Response received')
        response = port.read(port.in_waiting)
        # Print the raw response
        print(f'Response: {response.hex()}')
        # Now print the response in a more human-readable format
    else:
        print('No response received (timeout)')
