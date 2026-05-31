
import sys
import time

from struct import pack

from serial import Serial

from serialproto import SerialPort, CommandType

class AT28C:
    """Class representing an AT28C family EEPROM IC"""

    @property
    def port(self) -> Serial:
        return self._serial_port._port
    
    @property
    def max_address(self) -> int:
        return self._max_address
    
    @property
    def max_data_length(self) -> int:
        return 16  # Max data length is 16 bytes, due to the Arduino max input buffer

    def __init__(self, port: str):
        """
        Initialize the AT28C EEPROM programmer with the specified serial port.
        
        Args:
            port (str): The serial port to use for communication with the EEPROM programmer hardware.
        """

        self._serial_port = SerialPort(port)

        self._max_address: int = 0x01 # Default max address, should be overridden by subclasses

        pass

    def read_bytes(self, address: int, length: int) -> bytes:
        """
        Read a sequence of bytes from the specified address in the EEPROM.
        
        Args:
            address (int): The starting memory address to read from.
            length (int): The number of bytes to read.

        Returns:
            bytearray: The bytes read from the EEPROM.
        """

        # First check the address and length parameters to make sure they are valid
        if not (0 <= address <= self._max_address):
            raise ValueError(f"Address must be between 0x0000 and {self._max_address:#06X}, got {address:#06X}")
        if not (1 <= length <= self.max_data_length):
            raise ValueError(f"Length must be between 1 and {self.max_data_length}, got {length}")
        
        try:
            response: bytes = self._serial_port.read_bytes(address, length)
            return response
        except Exception as e:
            raise Exception(f"Error reading bytes from EEPROM: {e}")
    
    def write_bytes(self, address: int, data: bytearray) -> bytes:
        """
        Write a sequence of bytes to the specified address in the EEPROM.
        
        Args:
            address (int): The starting memory address to write to.
            data (bytearray): The data bytes to write to the EEPROM.
        """

        if not (0 <= address <= self._max_address):
            raise ValueError(f"Address must be between 0x0000 and {self._max_address:#06X}, got {address:#06X}")
        if not (1 <= len(data) <= self.max_data_length):
            raise ValueError(f"Length must be between 1 and {self.max_data_length}, got {len(data)}")
        
        try:
            response: bytes = self._serial_port.write_bytes(address, data)
            return response
        except Exception as e:
            raise Exception(f"Error writing bytes to EEPROM: {e}")
        
class AT28C256(AT28C):
    """Class representing an AT28C256 EEPROM IC"""

    MAX_ADDRESS: int = 0x7FFF  # 32KB - 1

    def __init__(self, port: str):
        super().__init__(port)
        self._max_address = self.MAX_ADDRESS

