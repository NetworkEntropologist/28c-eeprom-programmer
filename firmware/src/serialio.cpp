
#include <Arduino.h>
#include "defines.h"
#include "serialio.h"
#include "eeprom.h"

errorCode error = errorCode::OK;
validCommands current_command;
byte packet_data[MAX_PAYLOAD - 1];

byte in_buffer[MAX_PAYLOAD]; // +1 for the command byte
byte out_buffer[MAX_PAYLOAD]; // +1 for the length byte, +1 for the command byte

bool respond = false;

uint8_t calcChecksum(byte* data, uint8_t length) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; i++) {
        checksum += data[i];
    }
    return ~checksum; // Return the one's complement of the sum
}

uint8_t serialReceive(uint8_t length, bool Ack) {
    int l;
    
    do {
        l = Serial.readBytes(in_buffer, 24);
    } while (l == -1); 

    if (l > 0) {
        if (Serial.readBytes(in_buffer, min(l, length)) != 1) {
            error = errorCode::CORRUPT;
            return -1;
        }
    }

    if (Ack && serialSend(false) == -1) {
        error = errorCode::UNEXPECTED;
        return -1;
    }

    // Serial.write(l); // Echo the length of the received packet back to the sender for verification
    return l;
}

uint8_t serialSend(bool Ack) {
    // uint8_t length = out_buffer[0]; // Get length from the first byte of the buffer
    // if (length > 0) {
    //     Serial.write(out_buffer, length + 1);
    // }

    Serial.write(out_buffer, 24);

    if (Ack) {
        byte ackByte = 0x06; // ACK
        Serial.write(&ackByte, 1);
    }
    // Clear the output buffer after sending
    memset(out_buffer, 0, sizeof(out_buffer));
    return 0;
}

uint8_t sendErrorResponse() {
    byte error_command = ERRORRETURNBYTE;
    byte errorByte = static_cast<byte>(error);
    out_buffer[0] = error_command;
    out_buffer[1] = errorByte;
    return serialSend(false);
}

uint8_t parseCommand() {

    // Start by extracting the length byte and command bytes
    uint8_t start_char = in_buffer[0];
    byte command = in_buffer[1];
    

    // First check - is the packet length below the max payload size?
    // if (length > MAX_PAYLOAD) {
    //     error = errorCode::CORRUPT;
    //     return -1; // Invalid packet length
    // }

    // Now check if the command is valid and the payload length is correct
    // uint8_t packet_length = sizeof(in_buffer) - 2; // Total packet length minus startchar and length byte

    // if (length != packet_length) {
    //     error = errorCode::CORRUPT;
    //     return -1; // Invalid packet length
    // }

    // Last check - is the command byte valid?
    switch (command) {
        case static_cast<byte>(validCommands::READ):
            break;
        case static_cast<byte>(validCommands::WRITE):
            break;
        case static_cast<byte>(validCommands::DUMP):
            break;
        case static_cast<byte>(validCommands::ERASE):
            break;
        case static_cast<byte>(validCommands::LOAD):
            break;
        case static_cast<byte>(validCommands::RESET):
            break; 
        default:
            error = errorCode::UNEXPECTED;
            return -1; // Invalid command byte
    }

    current_command = static_cast<validCommands>(command);
    memcpy(packet_data, &in_buffer[2], 23);

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

    uint16_t address = (packet_data[3] << 8) | packet_data[4]; // Big-endian address from packet
    uint8_t data = readByte(address);

    out_buffer[0] = 0x01; // Length byte for response (1 byte of data)
    out_buffer[1] = 0x06; // ACK byte for response
    out_buffer[2] = 0x53; // Start of response in S Record format
    out_buffer[3] = 0x01; // S-Record type
    out_buffer[4] = 4; // Response byte count
    out_buffer[5] = address >> 8; // High byte of the address
    out_buffer[6] = address & 0xFF; // Low byte of the address
    out_buffer[7] = data; // Data byte

    for (int i = 8; i < 22; i++) {
        out_buffer[i] = 0; // The rest of the response is set to 0
    }

    // Checksums will be implemented in the future. For now we can simply set this to 0
    out_buffer[22] = 0;

    return serialSend(false); // Send response with ACK

}

uint8_t processWriteCommand() {

    uint16_t address = (packet_data[3] << 8) | packet_data[4]; // Big-endian address from packet
    uint8_t data = packet_data[5]; // Data byte to write
    writeByte(address, data);
    uint8_t written_data = readByte(address); // Read back the written data for verification
    if (written_data != data) {
        error = errorCode::CORRUPT;
        sendErrorResponse();
        return -1; // Write verification failed
    }
    
    // Build the response packet with the written data
    out_buffer[0] = 3; // Length byte for response, 2 byte for the address, 1 byte for the data at that address
    out_buffer[1] = address >> 8; // High byte of the address
    out_buffer[2] = address & 0xFF; // Low byte of the address
    out_buffer[3] = data; // Data byte

    return serialSend(true); // Send response with ACK

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
