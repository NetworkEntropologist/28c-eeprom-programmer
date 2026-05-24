
#ifndef SERIALIO_H
#define SERIALIO_H

#include <Arduino.h>
#include "defines.h"

#define ERRORRETURNBYTE 0x69 // A return command of 'E' means an error occurred

enum class errorCode {
    OK,
    RESET,
    CORRUPT,
    UNEXPECTED,
    UNKNOWN,
    NOTIMPLEMENTED
};

enum class validCommands {
    READ = 0x52, // 'R' for read
    WRITE = 0x57, // 'W' for write
    DUMP = 0x44, // 'D' for dump
    ERASE = 0x45, // 'E' for erase
    LOAD = 0x4C, // 'L' for load
    RESET = 0x18 // Reset command
};

extern errorCode error;
extern validCommands current_command;
extern byte packet_data[MAX_PAYLOAD -1];
extern byte in_buffer[MAX_PAYLOAD]; // Serial input buffer
extern byte out_buffer[MAX_PAYLOAD]; // Serial output buffer

extern bool respond; // Should the Arduino respond after receiving a command?

/**
 * @file serialio.h
 * @brief Serial communication functions for the 28C EEPROM programmer firmware
 * @details Implements a simple serial protocol for communication between the Arduino firmware and the Python control script.
 *          The protocol includes a command byte followed by an optional payload, with support for acknowledgments and error handling.
 *          The maximum payload size is defined in defines.h, and should be chosen carefully to avoid exceeding the Arduino's serial buffer limits.
 *          The serial communication is designed to be robust, with error codes defined for various failure scenarios.
 *          The activity LED on the Arduino is used to indicate when a serial command is being processed, providing visual feedback during programming operations.
 */
uint8_t serialReceive(uint8_t length, bool Ack);

/**
 * @brief Send data over the serial connection to the Python control script
 * @param Ack If true, the Arduino will send an ACK byte (0x06) after sending the data. If false, no ACK will be sent.
 * @return Returns 0 on success, or -1 on failure (e.g. if the serial connection is not available). In case of failure, the global error variable will be set to an appropriate error code. 
 */
uint8_t serialSend(bool Ack);

/**
 * @brief Send an error response over the serial connection to the Python control script
 * @param error The error code to send. This will be sent as a byte following the ERRORRETURNBYTE command byte.
 * @return Returns 0 on success, or -1 on failure (e.g. if the serial connection is not available). In case of failure, the global error variable will be set to an appropriate error code.
 */
uint8_t sendErrorResponse();

/**
 * @brief Parse a received command from the Python control script to confirm it is a valid command packet
 * @param buffer Pointer to the buffer containing the received command and its payload
 * @return Returns 0 if valid packet and -1 if invalid
 */
uint8_t parseCommand();

/**
 * @brief Build a response packet to send back to the Python control script after processing a command
 */
uint8_t buildResponse();

/**
 * @brief Process a received command from the Python control script
 * @return Returns 0 on success, or -1 on failure. In case of failure, the global error variable will be set to an appropriate error code.
 *          The specific processing for each command type (read, write, dump, erase, load, reset) will be handled in separate functions called by this main command processing function.
*/
 uint8_t processSerialCommand();

/**
 * @brief Process a received command from the Python control script
 * @param buffer Pointer to the buffer containing the received command and its payload
 */
void processSerialInput();

/**
* @brief Process a read command received from the Python control script
 * @param buffer Pointer to the buffer containing the read command and its payload
 * @return The byte read from the specified address, or -1 on failure. 
 * In case of failure, the global error variable will be set to an appropriate error code.
 */
uint8_t processReadCommand();

uint8_t processWriteCommand();

uint8_t processDumpCommand();

uint8_t processEraseCommand();

uint8_t processLoadCommand();

uint8_t processResetCommand();

#endif // SERIALIO_H