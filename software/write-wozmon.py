
import sys
import time

from struct import pack

from serial import Serial
from serial.tools import list_ports
from intelhex import IntelHex

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


port = Serial()
port.port = '/dev/cu.usbserial-14310'
port.baudrate = 115200
port.bytesize = 8           # 8 data bits (standard)
port.parity = 'N'           # No parity
port.stopbits = 1           # 1 stop bit
port.timeout = 1
port.dtr = False            # Prevents Arduino from resetting when opening the port
port.rts = False            # Disable RTS

file_path = '/Users/jaco/dev/6502/28c-eeprom-programmer/software/wozmon.hex'

hex_file = IntelHex()
hex_file.fromfile(file_path, format='hex')
hex_dict = hex_file.todict()

print('Opening port')
port.open()

print('Port opened')
while not port.is_open:
    pass

# Give the Arduino time to initialize after opening the port
time.sleep(2)

for address, data in hex_dict.items():
    print(f'Writing byte {data:02X} to EEPROM at address {address:04X}')
    cmd = build_write_command(address=address, data=data)
    port.write(cmd)
    time.sleep(0.01)  # Small delay between writes to avoid overwhelming the Arduino

for address, data in hex_dict.items():
    print(f'Reading byte from EEPROM at address {address:04X}')
    cmd = build_read_command(address=address)
    port.write(cmd)
    response = port.read(1)  # Read 1 byte response
    if response:
        print(f'Response: {response.hex().upper()}')
    else:
        print('No response received')
    time.sleep(0.01)  # Small delay between reads
