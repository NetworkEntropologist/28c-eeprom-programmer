
from intelhex import IntelHex

input_file = '/Users/jaco/dev/6502/28c-eeprom-programmer/software/wozmon.mon'
output_file = '/Users/jaco/dev/6502/28c-eeprom-programmer/software/wozmon.hex'
start_address = 0x0000
current_address = start_address

hex_file = IntelHex()

with open(input_file, 'r') as f:
    lines = f.readlines()

for line in lines:
    line = line[2:] # Strip the leading :
    line_bytes_str = line.split(' ')
    line_bytes = [int(byte_str, 16) for byte_str in line_bytes_str]

    for byte in line_bytes:
        hex_file[current_address] = byte
        current_address += 1

hex_file.tofile(output_file, format='hex')

