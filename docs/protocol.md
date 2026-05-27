
# 28C EEPROM Programmer Serial Protocol

## Overview

Communication between the Python host and Arduino firmware uses a fixed 25-byte packet format. The protocol combines a simple command structure with S-Record encoding for data transfers, providing both efficiency and data integrity.

**Total packet size:** 25 bytes (0x00-0x18)

## Packet Structure

```
+---------+---------+--------------------+----------+-------------------+
| Byte 0  | Byte 1  | Bytes 2 - N        | Byte N+1 | Bytes N+2 - 24    |
+---------+---------+--------------------+----------+-------------------+
| HEADER            | PAYLOAD            | CHECKSUM | PADDING           |
+---------+---------+--------------------+----------+-------------------+
| 0X01    | CMD     | VARIABLE LENGTH    | CHECKSUM | 0x00 (MAKE UP 25) |
+---------+---------+--------------------+----------+-------------------+
```

### Header (2 bytes)
- **Byte 0:** STARTCHAR = 0x01 (always)
- **Byte 1:** Command code (defines payload interpretation)

### Command (1 byte)
- Command determines the payload
- See below for list of commands and corresponding payloads

### Payload (variable, bytes 2-N)
- Interpretation depends on the command
- May contain S-Records or simple command data
- Includes any inner S-Record checksums

### Trailer (2 bytes)
- **Byte N+1:** Outer packet checksum (covers header + payload)
- **Bytes N+2-24:** Padding with 0x00 to reach 25 bytes total

## Checksum Calculation

### S-Record Checksum (inner)
Used within payload for data integrity:
1. Sum all bytes in the S-Record (excluding the checksum byte itself)
2. Take the least significant byte of the sum
3. Take the one's complement (~)

```c
uint8_t calc_srecord_checksum(byte* data, uint8_t length) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; i++) {
        checksum += data[i];
    }
    return ~checksum;  // One's complement
}
```

### Outer Packet Checksum
Covers the entire packet (header + payload + inner checksums):
1. Sum all bytes from byte 0 to byte N (the payload end)
2. Take the least significant byte
3. Take the one's complement

## Commands

### READ (0x52)
Read a single byte from the EEPROM.

**Request:**
```
Byte 0:    0x01 (STARTCHAR)
Byte 1:    0x52 (CMD = READ)
Bytes 2-3: Address (big-endian, 16-bit)
Byte 4:    Outer checksum
Bytes 5-24: Padding (0x00)
```

**Request Example** (read from address 0x0001):
```
01 52 00 01 [checksum] 00 00 ... 00
```

**Response:**
Arduino responds with S1 record containing the address and data read:
```
Byte 0:    0x01 (STARTCHAR)
Byte 1:    0x06 (CMD = ACK)
Bytes 2-N: S-Record (S1 format with address and data)
Byte N+1:  Outer checksum
Bytes N+2-24: Padding (0x00)
```

**Response Example** (0xA9 read from 0x0001):
```
01 06 S1 04 00 01 A9 [inner_checksum] 00 00 ... 00 [outer_checksum]
```

### WRITE (0x57)
Write data to the EEPROM.

**Request:**
```
Byte 0:    0x01 (STARTCHAR)
Byte 1:    0x57 (CMD = WRITE)
Bytes 2-N: S-Record (S1 format with address and data)
Byte N+1:  Outer checksum
Bytes N+2-24: Padding (0x00)
```

**Request Example** (write 0xA9 to address 0x0001):
```
01 57 S1 04 00 01 A9 [inner_checksum] 00 00 ... 00 [outer_checksum]
```

**Response:**
Verification response with S-Record of what was written:
```
01 06 S1 04 00 01 A9 [inner_checksum] 00 00 ... 00 [outer_checksum]
```

### DUMP (0x44)
Read multiple bytes from EEPROM.

**Request:**
```
Byte 0:    0x01 (STARTCHAR)
Byte 1:    0x44 (CMD = DUMP)
Bytes 2-3: Start address (big-endian, 16-bit)
Byte 4:    Number of bytes to read (1-16)
Byte 5:    Outer checksum
Bytes 6-24: Padding (0x00)
```

**Response:**
Bulk data returned as S-Records.

### ERASE (0x45)
Erase the EEPROM (command dependent on hardware capabilities).

**Request:**
```
Byte 0:    0x01 (STARTCHAR)
Byte 1:    0x45 (CMD = ERASE)
Byte 2:    Outer checksum
Bytes 3-24: Padding (0x00)
```

**Response:**
ACK on success or error response.

### RESET (0x18)
Soft reset of the Arduino firmware.

**Request:**
```
Byte 0:    0x01 (STARTCHAR)
Byte 1:    0x18 (CMD = RESET)
Byte 2:    Outer checksum
Bytes 3-24: Padding (0x00)
```

## S-Record Format (Payload)

When data is transferred, it's encoded as Motorola S-Records:

**S1 Record Structure:**
```
S1 <count> <address> <data...> <checksum>
```

- **S1:** Record type identifier (2 bytes, ASCII "S1")
- **count:** Byte count (1 byte) = address bytes + data bytes + checksum byte
- **address:** 2-byte address (big-endian)
- **data:** Variable payload data (0-16 bytes)
- **checksum:** S-Record checksum (1 byte)

**Example:** S1 04 0001 A9 CC
- S1 = record type
- 04 = 4 bytes (2 for address, 1 for data, 1 for checksum)
- 0001 = address
- A9 = data byte
- CC = checksum

## Error Responses

On error, the Arduino responds with an error packet:

```
01 69 <error_code> 00 00 ... 00 [outer_checksum] [padding]
```

- Byte 1: 0x69 ('E')
- Byte 2: Error code (defined in firmware)

## Summary Table

| Command | Code | Request Payload | Response Payload |
|---------|------|-----------------|------------------|
| READ    | 0x52 | Address (2 bytes) | S-Record with data |
| WRITE   | 0x57 | S-Record with address + data | S-Record verification |
| DUMP    | 0x44 | Address + count | S-Record(s) with bulk data |
| ERASE   | 0x45 | None | ACK or error |
| RESET   | 0x18 | None | ACK |