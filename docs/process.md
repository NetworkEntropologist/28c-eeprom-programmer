
## Arduino side

### General Process

1. Build command packet
2. Send command packet
3. Wait for ACK
4. Larther, rinse, repeat

### Building Command Packet

1. Look up command byte
2. Check if command requires S-Records
3. If the command requires an S-Record, build the S-Record
4. Build command packet
5. Send command packet

### Packets that require S-Records

1. Write command
2. Read response