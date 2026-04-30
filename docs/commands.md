# Serial Protocol

## Command reference

### Read a single byte
Read a single byte.

#### Message format

```
0x11 0x04 0x72 0x0401
```

| Byte | Notes |
| -- | -- |
| `0x11` | Start of message |
| `0x04` | Message length |
| `0x72` | Read Command |
| `0x0401` | Address to read |

#### Message response

```
0x11 0x04 0x76 0x0401 0xa9
```

| Byte | Notes |
| -- | -- |
| `0x11` | Start of message |
| `0x04` | Message length |
| `0x76` | Indicates a value follows |
| `0x0401` | Address |
| `0xa9` | Value at that address |

### Read a contigious block
Read a contigious block of addresses, up to a maximum of 16. if more than 16 is specified, the programmer will only return the first 16 bytes.

#### Message format

```
0x11 0x04 0x72 0x0401 0x0f
```

| Byte | Notes |
| -- | -- |
| `0x11` | Start of message |
| `0x04` | Message length |
| `0x72` | Read Command |
| `0x0401` | Start address |
| `0x0f` | Number of addresses |

#### Message response

```
0x11 0x04 0x76 0xa9 0x01 ... 0x85
```

| Byte | Notes |
| -- | -- |
| `0x11` | Start of message |
| `0x07` | Message length |
| `0x76` | Indicates a value follows |
| `0x01` | Value at first address |
| `0x85` | Value at last address |

### Write a byte
Message format:

```
0x11 0x04 0x77 0x0401 0xa9
```

| Byte | Notes |
| -- | -- |
| `0x11` | Start of message |
| `0x04` | Message length |
| `0x77` | Write Command |
| `0x0401` | Address to write |
| `0xa9` | Byte to write |


