
import sys
import time
import argparse

from struct import pack
from enum import Enum

from serial import Serial
from serial.tools import list_ports

port = Serial()
port.baudrate = 115200
port.bytesize = 8           # 8 data bits (standard)
port.parity = 'N'           # No parity
port.stopbits = 1           # 1 stop bit
port.timeout = 1
port.dtr = False            # Prevents Arduino from resetting when opening the port
port.rts = False            # Disable RTS

# eeprom_type = [
#     ['name' : '28C256', 'size' : 32768, 'page_size' : 64],
#     ['name' : '28C64', 'size' : 8192, 'page_size' : 32],
#     ['name' : '28C16', 'size' : 2048, 'page_size' : 16]
# ]

class EEPROMType(Enum):
    EEPROM_28C256 = '28C256'
    EEPROM_28C64 = '28C64'
    EEPROM_28C16 = '28C16'

file_path = ''
eeprom_type: EEPROMType # = EEPROMType.EEPROM_28C64
command = ''
addr = 0
data = 0
cmd_bytes = b''

def get_port() -> str:
    ports = list_ports.comports()
    
    if not ports:
        print('No serial ports found. Please specify a port using the -p or --port option.')
        sys.exit(1)
    elif len(ports) == 1:
        print(f'Auto selecting serial port: {ports[0].device}')
        return ports[0].device
    
    ports_dict = {}
    port_index = 1

    for port in ports:
        ports_dict[port_index] = port.device
        port_index += 1

    print("Multiple serial ports found. Please specify a port to use (Q to quit):")
    print()
    for index, device in ports_dict.items():
        print(f"{index}: {device}")
    print()

    valid_selection = False
    while not valid_selection:
        selection = input("Enter the number of the port to use (or Q to quit): ")
        if selection.upper() == 'Q':
            sys.exit(0)
        try:
            selection = int(selection)
            if selection in ports_dict:
                print(f'Selected port: {ports_dict[selection]}')
                valid_selection = True
                return ports_dict[selection]
            else:
                print(f"Invalid selection. Please enter a number between 1 and {len(ports_dict)}.")
        except ValueError:
            print(f"Invalid input. Please enter a number between 1 and {len(ports_dict)} or Q to quit.")

def get_type() -> EEPROMType:

    print('Please specify the EEPROM type to use (Q to quit):')
    print()
    for eeprom_type in EEPROMType:
        print(f"{eeprom_type.value}")
    print()

    valid_selection = False
    while not valid_selection:
        selection = input('Enter the EEPROM type to use (or Q to quit): ')
        if selection.upper() == 'Q':
            sys.exit(0)
        try:
            eeprom_type = EEPROMType(selection.upper())
            print(f'Selected EEPROM type: {eeprom_type.value}')
            valid_selection = True
            return eeprom_type
        except ValueError:
            print(f'Invalid selection.')

def get_command() -> str:
    
    print('Please specify the command to execute (Q to quit):')
    print()
    print('R: Read from specified EEPROM address')
    print('W: Write to specified EEPROM address')
    print()

    valid_selection = False
    while not valid_selection:
        selection = input('Enter the command to execute (or Q to quit): ')
        if selection.upper() == 'Q':
            sys.exit(0)
        elif selection.upper() in ['R', 'W']:
            print(f'Selected command: {selection.upper()}')
            valid_selection = True
            return selection.upper()
        else:
            print(f'Invalid selection. Please enter R or W.')

def get_address() -> int:
    valid_selection = False
    while not valid_selection:
        selection = input('Enter the address to read/write (hex) (or Q to quit): ')
        if selection.upper() == 'Q':
            sys.exit(0)
        try:
            address = int(selection, 16)
            print(f'Selected address: {hex(address)}')
            valid_selection = True
            return address
        except ValueError:
            print(f'Invalid selection. Please enter a valid hex address.')

def get_data() -> int:
    valid_selection = False
    while not valid_selection:
        selection = input('Enter the data to write (hex) (or Q to quit): ')
        if selection.upper() == 'Q':
            sys.exit(0)
        try:
            data = int(selection, 16)
            print(f'Selected data: {hex(data)}')
            valid_selection = True
            return data
        except ValueError:
            print(f'Invalid selection. Please enter a valid hex data byte.')

def valid_port(port_str: str) -> bool:
    ports_list = [port.device for port in list_ports.comports()]
    if port_str not in ports_list:
        print(f'{port_str} is not a valid serial port.')
        return False
    
    return True

def valid_type(eeprom_type: str) -> bool:
    if eeprom_type not in [e.value for e in EEPROMType]:
        print(f'{eeprom_type} is not a valid EEPROM type. Switching to interactive mode.')
        return False
    
    return True

def valid_command(command: str) -> bool:
    if command not in ['R', 'W', 'READ', 'WRITE']:
        print(f'{command} is not a valid command. Switching to interactive mode.')
        return False
    
    return True

def valid_address(eeprom_type: EEPROMType, address: str) -> bool:
    try:
        addr = int(address, 16)
    except ValueError:
        print(f'{address} is not a valid hex address. Switching to interactive mode.')
        return False

    # TODO: Get the min and max addresses from the EEPROM type dict instead of hard coding them here

    if eeprom_type.value == '28C256' and (addr < 0 or addr >= 0x8000):
        print(f'Address {hex(addr)} is out of range for EEPROM type {eeprom_type.value}. Switching to interactive mode.')
        return False
    elif eeprom_type.value == '28C64' and (addr < 0 or addr >= 0x2000):
        print(f'Address {hex(addr)} is out of range for EEPROM type {eeprom_type.value}. Switching to interactive mode.')
        return False
    elif eeprom_type.value == '28C16' and (addr < 0 or addr >= 0x800):
        print(f'Address {hex(addr)} is out of range for EEPROM type {eeprom_type.value}. Switching to interactive mode.')
        return False
    
    return True

def valid_data(data: str) -> bool:
    try:
        data_byte = int(data, 16)
    except ValueError:
        print(f'{data} is not a valid hex data byte. Switching to interactive mode.')
        return False

    if data_byte < 0 or data_byte > 0xFF:
        print(f'Data byte {hex(data_byte)} is out of range (0x00 - 0xFF). Switching to interactive mode.')
        return False
    
    return True

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

def parse_args(args):
    global command, addr, data, eeprom_type

    # No args specified so switch to interactive mode
    if len(sys.argv) == 1:
        print('No arguments specified. Switching to interactive mode.')
        # sys.exit(0) # TODO: Implement interactive mode

    serial_port = args.port
    if not serial_port or not valid_port(serial_port):
        print('No valid port specified. Switching to interactive mode.')
        serial_port = get_port()
    else:
        print(f'Using specified port: {serial_port}')
    
    port.port = serial_port

    dev_type = args.type.upper() if args.type else ''
    if not dev_type or not valid_type(dev_type):
        print('EEPROM type is required. Switching to interactive mode.')
        eeprom_type = get_type()
    else:
        eeprom_type = EEPROMType(dev_type)
        print(f'Using specified EEPROM type: {eeprom_type.value}')        

    cmd = args.command.upper() if args.command else ''
    print(f'Command specified: {cmd}')
    if not cmd or not valid_command(cmd):
        print('No valid command specified. Switching to interactive mode.')
        command = get_command()
    else:
        print(f'Using specified command: {cmd}')
        command = cmd

    addr = args.address
    if not addr or not valid_address(eeprom_type, addr):
        print('No valid address specified. Switching to interactive mode.')
        addr = get_address()
    else:
        addr = int(addr, 16)
        print(f'Using specified address: {hex(addr)}')

    if command == 'W':
        data_arg = args.data
        if not data_arg or not valid_data(data_arg):
            print('No valid data specified. Switching to interactive mode.')
            data = get_data()
        else:
            data = int(data_arg, 16)
            print(f'Using specified data: {hex(data)}')
    

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='EEPROM Programmer')
    
    parser.add_argument('-p', '--port', type=str, required=False, help='Serial port to use (e.g., /dev/cu.usbserial-14310, COM1, etc.)')
    parser.add_argument('-f', '--file', type=str, required=False, help='Path to the hex file to write to EEPROM')
    parser.add_argument('-c', '--command', type=str, required=False, help='Command to execute')
    parser.add_argument('-t', '--type', type=str, required=False, help='EEPROM type (e.g., 28C256, 28C64, etc.)')
    parser.add_argument('-a', '--address', type=str, required=False, help='Address to read/write (hex)')
    parser.add_argument('-d', '--data', type=str, required=False, help='Data byte to write (hex)')

    cmds = parser.add_subparsers(dest='command', help='Available commands')
    read_cmd = cmds.add_parser('read', help='Read from specified EEPROM address')
    # read_cmd = cmds.add_parser('r', help='Read from specified EEPROM address')
    write_cmd = cmds.add_parser('write', help='Write to specified EEPROM address')
    # write_cmd = cmds.add_parser('w', help='Write to specified EEPROM address')
    
    parse_args(parser.parse_args())

    proceed_input = input('Press enter to proceed or Q to quit: ')
    if proceed_input.upper() == 'Q':
        print('User aborted. Exiting.')
        sys.exit(0)

    print(f'Command: {command}, Address: {hex(addr)}, Data: {hex(data) if command == "W" else "N/A"}')

    match command:
        case 'READ' | 'R':
            cmd_bytes = build_read_command(addr)
        case 'WRITE' | 'W':
            cmd_bytes = build_write_command(addr, data)
        case _:
            print(f'Invalid command: {command}. Exiting.')
            sys.exit(1)

    print('Opening serial port...')
    port.open()
    time.sleep(2)  # Give the Arduino time to initialize after opening the port
    print('Serial port opened.')

    print(f'Sending command: {cmd_bytes.hex()}...')    
    port.write(cmd_bytes)
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



    