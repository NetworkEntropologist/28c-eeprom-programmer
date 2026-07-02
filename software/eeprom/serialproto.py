
import sys
import time

from struct import pack
from enum import Enum

from serial import Serial

from pysrecords import SRecord, SRecordType

class CommandType(Enum):
    READ = 0x52     # 'R' for read
    WRITE = 0x57    # 'W' for write
    DUMP = 0x44     # 'D' for dump
    LOAD = 0x4C     # 'L' for load
    RESET = 0x58    # 'X' for reset
    ERASE = 0x45    # 'E' for erase

class SerialPort:
    """Class representing the serial communication protocol for the EEPROM programmer hardware."""

    def __init__(self, port: str):
        """
        Initialize the serial port communication with the specified port.

        Args:
            port (str): The serial port to use for communication with the EEPROM programmer hardware.
        """

        self._port = Serial()
        self._port.port = port
        self._port.baudrate = 115200    # Baud rate is fixed for now
        self._port.bytesize = 8         # 8 data bits (standard)
        self._port.parity = 'N'         # No parity
        self._port.stopbits = 1         # 1 stop bit
        self._port.timeout = 1          # Default timeout
        self._port.dtr = False          # Prevents Arduino from resetting when opening the port
        self._port.rts = False          # Disable RTS

    def _calc_addr_bytes(self, address: int) -> bytes:
        """
        Calculate the high and low bytes of a 16-bit address.

        Args:
            address (int): The 16-bit address to split into high and low bytes.

        Returns:
            bytes: A byte array containing the high byte followed by the low byte of the address.
        """

        if not (0 <= address <= 0xFFFF):
            raise ValueError(f"Address must be a 16-bit value (0x0000 to 0xFFFF), got {address:#06X}")

        high_byte = (address >> 8) & 0xFF
        low_byte = address & 0xFF

        return bytes([high_byte, low_byte])

    def _calc_checksum(self, packet: bytes) -> int:
        """
        Calculate a simple checksum for the given packet.

        Args:
            packet (bytes): The byte array for which to calculate the checksum.
        Returns:
            int: The calculated checksum byte (0x00 to 0xFF).
        """
        checksum = sum(packet) & 0xFF  # Simple checksum: sum of all bytes modulo 256
        return checksum

    def _build_read_packet(self, address: int, length: int) -> bytes:
        """
        Build a read command packet to send to the EEPROM programmer hardware.

        Args:
            address (int): The memory address to read from.
            length (int): The number of bytes to read.
        Returns:
            bytes: The packed read command packet ready to be sent over the serial port.
        """

        return_bytes = bytearray(length)

        # Set the packet format: Big Endian, Start byte, Command code, Address (2 bytes), Length, Checksum, Padding
        format_string = f'>BBHBB19B' # Start byte (1 byte), Command Code (1 byte), Address (2 bytes), Length (1 byte), Checksum (1 byte), Padding (19 bytes)

        # Build the padding bytes
        padding = [0x00] * 19

        # Calculate the checksum for the first 5 bytes (start, command, address high, address low, length)
        temp_packet = bytearray([0x01, CommandType.READ.value]) + address.to_bytes(2, 'big') + length.to_bytes(1, 'big')
        checksum = self._calc_checksum(temp_packet)

        # Pack and return the command
        return pack(format_string, 0x01, CommandType.READ.value, address, length, checksum, *padding)

    def _build_write_packet(self, address: int, data: bytes) -> bytes:
        """
        Build a write command packet to send to the EEPROM programmer hardware.

        Args:
            address (int): The memory address to write to.
            data (int): The data byte to write.
        Returns:
            bytes: The packed write command packet ready to be sent over the serial port.
        """

        response: bytes = bytearray(1)

        # To calculate the number of padding bytes required:
        # 1. Start byte + Command byte = 2 bytes
        # 2. S-Record length: 1 byte for length, 2 bytes for address, variable bytes for data (up to 16), 1 byte for checksum
        # 3. Total so far: 2 + 1 + 2 + data length + 1 = 6 + data length
        # 4. Total packet length must be 25 bytes, so padding = 25 - (6 + data length) = 19 - data length
        padding = [0x00] * (19 - len(data))

        # Set the packet format: Big Endian, Start byte, Command code, S-Records, Checksum, Padding
        format_string = f'>BBHBB{len(padding)}B' # Start byte (1 byte), Command Code (1 byte), Address (2 bytes), Data (1 byte), Checksum (1 byte), Padding (19 bytes)

        # Build the S-Record
        s_record = SRecord(SRecordType.S1, address, data)

        # And finally, build the packet
        temp_packet = bytearray([0x01, CommandType.WRITE.value]) + s_record._packet
        checksum = self._calc_checksum(temp_packet)
        return pack(format_string, 0x01, CommandType.WRITE.value, address, data[0], checksum, *padding)

    def _build_dump_command(self) -> bytes:
        """
        Build a dump command packet to send to the EEPROM programmer hardware.

        Returns:
            bytes: The packed dump command packet ready to be sent over the serial port.
        """

        raise NotImplementedError("Dump command not implemented yet")

    def _build_load_command(self) -> bytes:
        """
        Build a load command packet to send to the EEPROM programmer hardware.

        Returns:
            bytes: The packed load command packet ready to be sent over the serial port.
        """

        raise NotImplementedError("Load command not implemented yet")

    def _build_erase_command(self) -> bytes:
        """
        Build an erase command packet to send to the EEPROM programmer hardware.

        Returns:
            bytes: The packed erase command packet ready to be sent over the serial port.
        """

        raise NotImplementedError("Erase command not implemented yet")

    def _build_reset_command(self) -> bytes:
        """
        Build a reset command packet to send to the EEPROM programmer hardware.

        Returns:
            bytes: The packed reset command packet ready to be sent over the serial port.
        """

        raise NotImplementedError("Reset command not implemented yet")

    def _do_command(self, packet: bytes) -> bytes:
        """
        Send a command packet to the EEPROM programmer hardware and read the response.

        Args:
            packet (bytes): The command packet to send.
        Returns:
            bytes: The response received from the EEPROM programmer hardware.
        """

        try:
            
            self._port.open()
            self._port.write(packet)
            self._port.flush()  # Ensure data is sent immediately
            time.sleep(0.5)  # Give Arduino time to process the command

            if self._port.in_waiting > 0:
                response = self._port.read(self._port.in_waiting)
                return response
            else:
                raise RuntimeError("No response received (timeout)")
        
        except RuntimeError as e:
            raise RuntimeError(f'Error sending command: {e}')

    def read_bytes(self, address: int, length: int) -> bytes:
        """
        Send a read command to the EEPROM programmer to read a sequence of bytes from the specified address in the EEPROM.
        
        Args:
            address (int): The starting memory address to read from.
            length (int): The number of bytes to read.

        Returns:
            bytearray: The bytes read from the EEPROM.
        """
        # read_packet = self._build_read_packet(address, length)

        response: bytes = self._do_command(self._build_read_packet(address, length))

        return response

    def write_bytes(self, address: int, data: bytearray) -> bytes:
        """
        Send a write command to the EEPROM programmer to write a sequence of bytes to the specified address in the EEPROM.
        
        Args:
            address (int): The starting memory address to write to.
            data (bytearray): The data bytes to write to the EEPROM.

        Returns:
            bytearray: The response received from the EEPROM programmer hardware after writing.
        """

        write_packet = self._build_write_packet(address, data)
        response: bytes = self._do_command(write_packet)
        return response
