# Serial Protocol - EEPROM Programmer

## Overview

The protocol uses a simple frame-based structure with the following components:
- **START**: Frame start marker
- **LENGTH**: Number of payload bytes that follow (command + data)
- **COMMAND**: The operation to perform (READ, WRITE, etc.)
- **PAYLOAD**: Command-specific data (addresses, values, etc.)

## Packet Structure

### General Frame Format
```
[START] [LENGTH] [CMD] [PAYLOAD...]
```

| Field | Size | Value | Notes |
|-------|------|-------|-------|
| START | 1 byte | 0x01 | Frame start marker |
| LENGTH | 1 byte | variable | Number of bytes following (CMD + PAYLOAD) |
| CMD | 1 byte | variable | Command byte |
| PAYLOAD | variable | variable | Command-specific data |

---

## Commands

### READ - Read a single byte from EEPROM

#### Request
```
[0x01] [0x03] [0x52] [ADDR_H] [ADDR_L]
```

| Byte | Value | Notes |
|------|-------|-------|
| B0 | 0x01 | START |
| B1 | 0x03 | LENGTH (command + 2 address bytes) |
| B2 | 0x52 | READ command ('R') |
| B3 | - | Address high byte |
| B4 | - | Address low byte |

#### Response
```
[0x01] [0x03] [ADDR_H] [ADDR_L] [DATA]
```

| Byte | Value | Notes |
|------|-------|-------|
| B0 | 0x01 | START |
| B1 | 0x03 | LENGTH (2 address bytes + 1 data byte) |
| B2 | - | Address high byte (echoed) |
| B3 | - | Address low byte (echoed) |
| B4 | - | Data byte read from EEPROM |

**Example**: Read address 0x0001
- Request: `01 03 52 00 01`
- Response: `01 03 00 01 A9` (reads value 0xA9 from address 0x0001)

---

### WRITE - Write a single byte to EEPROM

#### Request
```
[0x01] [0x04] [0x57] [ADDR_H] [ADDR_L] [DATA]
```

| Byte | Value | Notes |
|------|-------|-------|
| B0 | 0x01 | START |
| B1 | 0x04 | LENGTH (command + 2 address bytes + 1 data byte) |
| B2 | 0x57 | WRITE command ('W') |
| B3 | - | Address high byte |
| B4 | - | Address low byte |
| B5 | - | Data byte to write |

#### Response
```
[0x01] [0x03] [ADDR_H] [ADDR_L] [DATA]
```

| Byte | Value | Notes |
|------|-------|-------|
| B0 | 0x01 | START |
| B1 | 0x03 | LENGTH (2 address bytes + 1 data byte) |
| B2 | - | Address high byte (echoed) |
| B3 | - | Address low byte (echoed) |
| B4 | - | Data byte written |

**Example**: Write 0xA9 to address 0x0001
- Request: `01 04 57 00 01 A9`
- Response: `01 03 00 01 A9` (confirms write)

---

## Error Response

### Error Format
```
[0x06] [0x69] [ERROR_CODE]
```

| Byte | Value | Notes |
|------|-------|-------|
| B0 | 0x06 | ACK (acknowledgment) |
| B1 | 0x69 | Error marker ('i') |
| B2 | - | Error code |

### Error Codes
| Code | Name | Meaning |
|------|------|---------|
| 0 | OK | No error |
| 1 | RESET | Reset condition |
| 2 | CORRUPT | Corrupted data received |
| 3 | UNEXPECTED | Unexpected command or state |
| 4 | UNKNOWN | Unknown error |
| 5 | NOTIMPLEMENTED | Command not yet implemented |

---

## Notes

- All multi-byte values (addresses) are in **big-endian** format (high byte first)
- The LENGTH field includes the command byte plus all payload bytes
- Responses echo back the address for verification purposes
- Future commands (DUMP, ERASE, LOAD) follow the same frame structure with command-specific payloads
