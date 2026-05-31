

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

def calc_checksum(data: bytes) -> bytes:
    
    # Not implemented, so return 0 for now
    return b'\x00'

"""Returns a byte array representing the S-Record for the given data"""
def build_srecord(length: int, address: int, data: bytearray) -> bytes:
    # S-record format: SLLAAAADDDD...DDCC
    # LL = record length (number of bytes in the record, including address and data)
    # AAAA = address (2 bytes for S1 records)
    # DDDD...DD = data bytes
    # CC = checksum (1 byte, calculated as the least significant byte of the sum of all bytes in the record)
    # Note that the max number of data bytes is 16, due to the Arduino max input buffer

    # Calculate the record length (address bytes + data bytes + checksum byte)
    record_length = 2 + length + 1  # 2 bytes for address, length of data, and 1 byte for checksum
    if record_length > 19:  # Max record length is 19 bytes (2 for address, 16 for data, 1 for checksum)
        raise ValueError('Data length exceeds maximum allowed for S-record')
    
    # Build the S-record
    srecord = bytearray()
    srecord.append(ord('S'))  # Start of S-record
    srecord.append(ord('1'))  # S1 record type (16-bit address)
    srecord.append(record_length)  # Record length
    srecord.append((address >> 8) & 0xFF)  # High byte of address
    srecord.append(address & 0xFF)  # Low byte of address
    srecord.extend(data)  # Data bytes
    srecord.append(0x00)  # Checksum byte (not implemented, so always 0)
    return bytes(srecord)

def build_command(**kwargs):

    cmd = bytearray(24)
    length: int = 0

    # Command format: [length, command, address_high, address_low, data]
    # Command needs to be sent as a byte array, so we construct it accordingly

    format_string = ">B B 2s B H 16s B"

    cmd_byte: int
    srecord_type: bytes
    length: int
    address: int
    data: bytes
    checksum: int
        
    if 'command' in kwargs:
        # Set command byte to command code
    

        if kwargs['command'] == 'R':

            cmd_byte = 'R'.encode('utf-8')[0]  # Command code for read
            srecord_type = b'\x00\x00'  # Not used for read commands, but reserved for future use
            length = 0 # Length of data (not used for read commands, but reserved for future use)
            address = kwargs['address']  # Address to read from
            data = b'\x00' * 16  # Data bytes (not used for read commands, but reserved for future use)
            checksum = 0x00 # Checksum byte (not implemented yet, so always 0x00)


            # Let's build a packet for the read command. Packet format is loosely based on S-Records, 
            # and follows this structure:
            # [Start byte (0x01), Command code (1 byte), 0x00, 0x00, 0x00
            # Address high byte, Address low byte
            # 16x 0x00 bytes for data (not used for read commands, but reserved for future use)
            # Checksum byte (not implemented yet, so always 0x00)]

            # length = 24
            # cmd[0] = 0x01  # Start byte (always 0x01)
            # cmd[1] = 'R'.encode('utf-8')[0] # Command code for read

            # for i in range(2, 4):
            #     cmd[i] = 0x00  # Always 0x00 for read commands

            # cmd[4] = (kwargs['address'] >> 8) & 0xFF  # High byte of address
            # cmd[5] = kwargs['address'] & 0xFF  # Low byte of address
            
            # for i in range(6, 23):
            #     cmd[i] = 0x00  # Always 0x00 for read commands

            # cmd[23] = 0x00  # Checksum byte (not implemented yet, so always 0x00)

        elif kwargs['command'] == 'W':

            cmd_byte = 'W'.encode('utf-8')[0]  # Command code for read
            srecord_type = b'S1'  # Not used for read commands, but reserved for future use
            length = 4 # Payload length (address + data + checksum)
            address = kwargs['address']  # Address to read from
            data = kwargs['data'] + b'\x00' * 15  # Data bytes (only writing 1 byte, so pad the rest with 0x00)
            checksum = 0x00 # Checksum byte (not implemented yet, so always 0x00)

            # length = 24
            # cmd.append(0x01)  # Start byte (always 0x01)
            # cmd.append('W'.encode('utf-8')[0])  # Command code for write

            # # Calculate an S-Record for the data to write
            # s_record = build_srecord(length=1, address=kwargs['address'], data=bytearray([kwargs['data']]))
            # cmd.extend(s_record)  # Append the S-record to the command

            # # Calculate the length and pad with 0x00 to fill up the packet
            # while len(cmd) < 24:
            #     cmd.append(0x00)  # Pad with 0x00 to fill up the packet

            # # Finally calculate the checksum (not implemented yet, so always 0x00)
            # cmd.append(0x00)  # Checksum byte (not implemented yet, so always 0x00)
    
    return pack(format_string, 0x01, cmd_byte, srecord_type, length, address, data, checksum)


while True:
    
    command = input('Enter command (R=read, W=write, D=dump, E=erase, L=load, X=reset, Q=quit): ').strip().upper()

    cmd = bytearray()

    if command == 'Q':
        print('Exiting...')
        port.close()
        sys.exit(0)
    elif command == 'W':
        address = int(input('Enter address to write (hex): '), 16)
        data = int(input('Enter data to write (hex): '), 16)
        cmd = build_command(command=command, address=address, data=data)
        
    elif command == 'R':
        address = int(input('Enter address to read (hex): '), 16)
        cmd = build_command(command=command, address=address)
        
    print('Sending command')

    port.write(cmd)
    port.flush()  # Ensure data is sent immediately

    time.sleep(0.5)  # Give Arduino time to process the command

    while port.in_waiting < 24:
        pass

    if port.in_waiting > 0:
        print('Response received')
        response = port.read(port.in_waiting)
        # Print the raw response
        print(f'Response: {response.hex()}')
        # Now print the response in a more human-readable format
    else:
        print('No response received (timeout)')



