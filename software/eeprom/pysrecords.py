
from enum import Enum
from struct import pack

class SRecordType(Enum):
    """
    Enum representing valid S-record types.
    
    S-record types are used to categorize different types of records in the
    Motorola S-record format, used for encoding programs and data.
    
    Attributes:
        S0: Header record containing file metadata and description.
        S1: Data record with 16-bit address.
        S2: Data record with 24-bit address.
        S3: Data record with 32-bit address.
        S5: Count record specifying the number of S1/S2/S3 records.
        S7: Start address record with 32-bit address (execution start point).
        S8: Start address record with 24-bit address (execution start point).
        S9: Start address record with 16-bit address (execution start point).
    """
    S0 = 0
    S1 = 1
    S2 = 2
    S3 = 3
    S5 = 5
    S7 = 7
    S8 = 8
    S9 = 9

class SRecord:

    @property
    def record_type(self) -> SRecordType:
        return self._record_type
    @record_type.setter
    def record_type(self, value: SRecordType):
        self._record_type = value

    @property
    def address(self) -> int:
        return self._address
    @address.setter
    def address(self, value: int):
        self._address = value

    @property
    def data(self) -> bytes:
        return self._data
    @data.setter
    def data(self, value: bytearray):
        self._data = value

    @property
    def byte_array(self) -> bytearray:
        return self._byte_array
    
    @property
    def packet(self) -> bytes:
        return self._packet


    def __init__(self, record_type: SRecordType, address: int, data: bytes):
        """
        Initialize an S-record with the given parameters.
        
        Args:
            record_type (SRecordType): The type of S-record.
            address (int): The start address for this record.
            data (bytearray): The data bytes to store in this record.
        """

        self._record_type = record_type
        self._address = address
        self._data = data

        self._length = len(data) + 3  # 1 byte for byte count, 2 bytes for address
        self._data_length = len(data)

        # Packet format: Big Endian, S-Record, Type, Length, Address, Data (1 to 16 bytes), Checksum
        self._packet_format = f'>cBBH{self._data_length}BB' 

        self._checksum = self._calc_checksum()

        self._packet = self._build_packet()

        self._byte_array = self._calc_srecord()

    def __str__(self):

        # TODO: This is just a framework for now.

        # Convert the record to a string in S-record format
        byte_count = len(self.data) + 3  # 1 byte for byte count, 2 bytes for address
        record_str = f"S{self.record_type.value}{byte_count:02X}{self.address:04X}"
        for byte in self.data:
            record_str += f"{byte:02X}"
        checksum = (byte_count + (self.address >> 8) + (self.address & 0xFF) + sum(self.data)) & 0xFF
        checksum = (~checksum) & 0xFF  # One's complement
        record_str += f"{checksum:02X}"
        return record_str
    
    def __repr__(self):
        return f"SRecord(record_type={self.record_type}, address={self.address}, data={self.data})"
    
    def _calc_srecord(self) -> bytearray:
        byte_array = bytearray()
        byte_array.append(self._length)
        byte_array.append((self._address >> 8) & 0xFF)  # High byte of address
        byte_array.append(self._address & 0xFF)         # Low byte of address
        byte_array.extend(self._data)
        checksum = (self._length + (self._address >> 8) + (self._address & 0xFF) + sum(self._data)) & 0xFF
        checksum = (~checksum) & 0xFF  # One's complement
        byte_array.append(checksum)
        return byte_array

    def _calc_checksum(self) -> int:
        checksum = (self._length + (self._address >> 8) + (self._address & 0xFF) + sum(self._data)) & 0xFF
        checksum = (~checksum) & 0xFF  # One's complement
        return checksum

    def _build_packet(self) -> bytes:
        return pack(self._packet_format, b'S', self.record_type.value, self._length, self._address, *self._data, self._checksum)

    def indexed_data(self, position: int) -> int:
        if 0 <= position < len(self._data):
            return self._data[position]
        else:
            raise IndexError("Position out of range for this S-record")
    
    def address_data(self, address: int) -> int:
        offset = address - self._address
        if 0 <= offset < len(self._data):
            return self._data[offset]
        else:
            raise IndexError("Address out of range for this S-record")