
from intelhex import IntelHex

hex_file = IntelHex()
hex_file.fromfile('/Users/jaco/dev/6502/28c-eeprom-programmer/software/wozmon.hex', format='hex')

hex_dict = hex_file.todict()

# To generate the output format take the raw dict and parse it byte by byte, contstructing 16 byte lines in groups of 8 bytes
# The output format is:
# AAAA DD DD DD DD DD DD DD DD  DD DD DD DD DD DD DD DD ....

# First get the total number of bytes and calculate the number of lines needeed
total_bytes = len(hex_dict)
lines_needed = (total_bytes + 15) // 16

current_address = 0

formatted_output = ''

for line in range(lines_needed):
    line_bytes = []
    for i in range(16):
        byte_value = hex_dict.get(current_address + i, 0xFF)  # Default to 0xFF if address is not in the dict
        line_bytes.append(byte_value)
    
    # Format the line output
    line_output = f"{current_address:04X} " + " "
    for i in range (8):
        line_output += f"{line_bytes[i]:02X} "
    line_output += " "  # Extra space between the two groups of 8 bytes
    for i in range(8, 16):
        line_output += f"{line_bytes[i]:02X} "
    
    # And finally add the ASCII representation of the bytes at the end of the line
    ascii_representation = ''.join(chr(b) if 32 <= b <= 126 else '.' for b in line_bytes)
    line_output += " " + ascii_representation

    print(line_output)
    
    current_address += 16



