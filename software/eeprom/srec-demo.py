
from srecords import SRecord, SRecordType

s_record = SRecord(SRecordType.S1, 0x1234, bytearray([0xDE, 0xAD, 0xBE, 0xEF, 0xA9, 0x5C, 0x42, 0x00, 0xFF, 0xEE, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66]))

print(s_record)
print(s_record.byte_array)
print(f"Checksum: {s_record._calc_checksum():02X}")
print(f"Packet: {s_record.packet.hex().upper()}")
