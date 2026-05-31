

from pysrecords import SRecord, SRecordType
from at28c import AT28C256

print('Reading address 0x0000 from AT28C256 EEPROM...')

eeprom = AT28C256(port='/dev/cu.usbmodem5C372192251')

# print(f'Value at address 0x0000: {eeprom.read_bytes(0x0000, 1).hex()}')
print(f'Writing value 0xAB sixteen times starting at address 0x0000...')
eeprom.write_bytes(0x0000, bytearray([0xAB] * 16))
