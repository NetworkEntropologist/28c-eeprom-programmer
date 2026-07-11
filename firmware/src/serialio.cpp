
#include <Arduino.h>
#include "defines.h"
#include "serialio.h"
#include "eeprom.h"

errorCode error = errorCode::OK;
validCommands current_command;
byte packet_data[MAX_PAYLOAD - 1];

// byte in_buffer[MAX_PAYLOAD]; // +1 for the command byte
byte out_buffer[MAX_PAYLOAD]; // +1 for the length byte, +1 for the command byte

bool respond = false;

uint8_t calcChecksum(byte* data, uint8_t length) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; i++) {
        checksum += data[i];
    }
    return ~checksum; // Return the one's complement of the sum
}

uint8_t serialReceive(byte *buffer, uint8_t bufferSize, bool Ack) {
    int frameStart;

    // Read and validate frame start byte (0x01)
    do {
        frameStart = Serial.read();
    } while (frameStart == -1);  // Wait for frame start

    if (frameStart != 0x01) {
        error = errorCode::CORRUPT;
        return -1;  // Invalid frame start
    }

    // Frame start is valid, store it in buffer
    buffer[0] = frameStart;

    // Read the length byte, which tells us exactly how many bytes follow
    // (command + payload). Packets are variable length depending on command
    // (e.g. read sends 3, write sends 4), so this must be read before we
    // know how many more bytes to wait for.
    if (Serial.readBytes(&buffer[1], 1) != 1) {
        error = errorCode::CORRUPT;
        return -1;
    }

    uint8_t payloadLength = buffer[1];
    if (payloadLength > bufferSize - 2) {
        error = errorCode::CORRUPT;
        return -1;  // Payload too large for the provided buffer
    }

    int bytesRead = Serial.readBytes(&buffer[2], payloadLength);

    if (bytesRead != payloadLength) {
        error = errorCode::CORRUPT;
        return -1;
    }

    // Send ACK if requested
    if (Ack) {
        byte ackByte = 0x06;  // ACK
        Serial.write(&ackByte, 1);
    }

    // Return total bytes read (frame start + length byte + payload)
    return bytesRead + 2;
}

uint8_t serialSend(byte *buffer, uint8_t length, bool Ack) {
    // uint8_t length = out_buffer[0]; // Get length from the first byte of the buffer
    // if (length > 0) {
    //     Serial.write(out_buffer, length + 1);
    // }

    Serial.write(buffer, length); // Write the buffer to the UART

    // if (Ack) {
    //     byte ackByte = 0x06; // ACK
    //     Serial.write(&ackByte, 1);
    // }
    // Clear the output buffer after sending
    // memset(out_buffer, 0, sizeof(out_buffer));
    return 0;
}

uint8_t sendErrorResponse() {
    byte error_command = ERRORRETURNBYTE;
    byte errorByte = static_cast<byte>(error);
    out_buffer[0] = error_command;
    out_buffer[1] = errorByte;
    return serialSend(out_buffer, 2, false);
}

uint8_t verifyCommand(byte *buffer) {

    // Start by extracting the length byte and command bytes
    // uint8_t start_char = buffer[0];
    byte command = buffer[2];
    
    switch (command) {
        case static_cast<byte>(validCommands::READ):
            current_command = static_cast<validCommands>(command);
            memcpy(packet_data, &buffer[3], 2);
            break;
        case static_cast<byte>(validCommands::WRITE):
            current_command = static_cast<validCommands>(command);
            memcpy(packet_data, &buffer[3], 3);
            break;
        case static_cast<byte>(validCommands::DUMP):
            current_command = static_cast<validCommands>(command);
            break;
        case static_cast<byte>(validCommands::ERASE):
            current_command = static_cast<validCommands>(command);
            break;
        case static_cast<byte>(validCommands::LOAD):
            current_command = static_cast<validCommands>(command);
            break;
        case static_cast<byte>(validCommands::RESET):
            current_command = static_cast<validCommands>(command);
            break; 
        default:
            error = errorCode::UNEXPECTED;
            return -1; // Invalid command byte
    }

    return 0; // Valid packet
}

uint8_t buildResponse() {
    return 0;
}

uint8_t processSerialCommand() {

    switch (current_command) {

        case validCommands::READ:
            return processReadCommand();
            break;
        
        case validCommands::WRITE:
            return processWriteCommand();
            break;
        
        case validCommands::DUMP: // 'd' for dump
            return processDumpCommand();
            break;

        case validCommands::ERASE: // 'e' for erase
            return processEraseCommand();
            break;
        
        case validCommands::LOAD: // 'l' for load
            return processLoadCommand();
            break;
        
        case validCommands::RESET: // Reset command
            return processResetCommand();
            break;

    }
    return 0;
}

uint8_t processReadCommand() {

    uint16_t address = (packet_data[0] << 8) | packet_data[1]; // Big-endian address from packet
    uint8_t data = readByte(address);

    byte buffer[5];

    buffer[0] = 0x01; // Start byte
    buffer[1] = 0x03; // Payload length
    buffer[2] = address >> 8; // High byte of the address
    buffer[3] = address & 0xFF; // Low byte of the address
    buffer[4] = data; // Data byte read from the EEPROM
    
    return serialSend(buffer, 5, false); // Send response with ACK

}

uint8_t processWriteCommand() {

    uint16_t address = (packet_data[0] << 8) | packet_data[1]; // Big-endian address from packet
    uint8_t data = packet_data[2]; // Data byte to write
    writeByte(address, data);
    
    // Add delay to allow EEPROM to stabilize after write before verification read
    delayMicroseconds(1000); // 1ms additional delay for data stabilization
    
    // Build the response packet with the written data
    byte buffer[5];
    buffer[0] = 0x01; // Start byte
    buffer[1] = 0x03; // Payload length
    buffer[2] = address >> 8; // High byte of the address
    buffer[3] = address & 0xFF; // Low byte of the address
    buffer[4] = data; // Data byte

    return serialSend(buffer, 5, false); // Send response with ACK

}

uint8_t processDumpCommand() {

    error = errorCode::NOTIMPLEMENTED;
    return -1;

}

uint8_t processEraseCommand() {

    error = errorCode::NOTIMPLEMENTED;
    return -1;

}

uint8_t processLoadCommand() {

    error = errorCode::NOTIMPLEMENTED;
    return -1;

}

uint8_t processResetCommand() {

    error = errorCode::NOTIMPLEMENTED;
    return -1;

}
