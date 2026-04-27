/**
 * @file defines.h
 * @brief Pin definitions and timing constants for 28Cx EEPROM programmer
 * @details Configure these values based on your hardware design and EEPROM requirements.
 *          Pin assignments: Update based on your Arduino connections.
 *          Timing parameters: Consult the specific EEPROM datasheet for accurate values.
 */

#ifndef DEFINES_H
#define DEFINES_H

// EEPROM Control Pins
#define ROM_WE 14 // Arduino pin D14 (A0) connected to EEPROM /WE (active low)
#define ROM_OE 15 // Arduino pin D15 (A1) connected to EEPROM /OE (active low)
#define ROM_CE 16 // Arduino pin D16 (A2) connected to EEPROM /CE (active low)

// EEPROM Data Pins
#define D0 2 // Arduino pin D2 connected to EEPROM D0
#define D7 9 // Arduino pin D9 connected to EEPROM D7

// 74HC595 Shift Register Control Pins
#define SRCLK 12 // Arduino pin D12 connected to 74HC595 SRCLK - pin 11
#define SRCLR 17 // Arduino pin D17 (A3) connected to 74HC595 SRCLR - pin 10 (active low)
#define RCLK 10 // Arduino pin D10 connected to 74HC595 RCLK - pin 12
#define SEROE 18 // Arduino pin D18 (A4) connected to 74HC595 SEROE - pin 13 (active low)
#define DATA 11 // Arduino pin D11 connected to 74HC595 SER - pin 14

// Timing Parameters
#define PROGDELAY 100 // Delay in microseconds for programming operations
#define READDELAY 50 // Delay in microseconds for read operations
#define ADDRDELAY 50 // Delay in microseconds for address setup
#define DATADELAY 50 // Delay in microseconds for data setup
#define GENDELAY 100 // General delay in microseconds

#define MAX_ADDRESS 0x7FFF // Maximum address for 28C512 EEPROM (32KB)

// Debug parameters
extern bool DEBUG_MODE; // Global variable to control debug mode
#define DEBUG_DATA 0xAA // Test data value to write during address test loop
#define DEBUG_DELAY 50 // Delay in milliseconds for debugging loops to allow visual verification of blinkenlights

// Serial comms parameters
#define SERIAL_BAUD 115200 // Baud rate for serial communication with Python control script

#endif // DEFINES_H
