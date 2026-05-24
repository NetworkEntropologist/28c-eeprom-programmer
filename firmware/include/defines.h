/**
 * @file defines.h
 * @brief Definitions and constants for the 28C EEPROM programmer firmware
 * @details Configure these values based on your hardware design and EEPROM requirements.
 *          Pin assignments: Update based on your Arduino connections.
 *          Timing parameters: Consult the specific EEPROM datasheet for accurate values.
 */

#ifndef DEFINES_H
#define DEFINES_H

// EEPROM Control Pins
#define ROM_WE 14 // EEPROM /WE (Write Enable) pin, defaults to Arduino pin D14 (A0)
#define ROM_OE 15 // EEPROM /OE (Output Enable) pin, defaults to Arduino pin D15 (A1)
#define ROM_CE 16 // EEPROM /CE (Chip Enable) pin, defaults to Arduino pin D16 (A2)

// EEPROM Data Pins
#define D0 2 // EEPROM data bus pin D0, defaults to Arduino pin D2
#define D7 9 // EEPROM data bus pin D7, defaults to Arduino pin D9

// 74HC595 Shift Register Control Pins
#define SRCLK 12 // 74HC595 shift register clock pin, defaults to Arduino pin D12
#define SRCLR 17 // 74HC595 shift register clear pin, defaults to Arduino pin D17 (A3)
#define RCLK 10 // 74HC595 shift register latch pin, defaults to Arduino pin D10
#define SEROE 18 // 74HC595 shift register output enable pin, defaults to Arduino pin D18 (A4)
#define DATA 11 // 74HC595 shift register data pin, defaults to Arduino pin D11

// Timing Parameters
#define WRITE_DELAY 10 // Delay in microseconds for programming operations
#define READ_DELAY 1 // Delay in microseconds for read operations
#define ADDR_DELAY 1 // Delay in microseconds for address setup
#define DATA_DELAY 1 // Delay in microseconds for data setup
#define GEN_DELAY 10 // General delay in microseconds

// 28Cxx EEPROM timings from datasheet. Only adjust these if you know EXACTLY what you are doing!
// These are originally specified in ns, but since the Arduino does not normally support sub-microsecond timing, 
// these are converted to microseconds, but the values are still entirely based on the datasheet figures.
#define tAS 1 // Address setup time in microseconds
#define tOES 1 // Output enable setup time in microseconds
#define tAH 5 // Address hold time in microseconds
#define tWP 10 // Write pulse width in microseconds
#define tDS 5 // Data setup time in microseconds
#define tDH 5 // Data hold time in microseconds
#define tOEH 1 // Output enable hold time in microseconds
#define tCS 1 // Chip select setup time in microseconds
#define tCH 1 // Chip select hold time in microseconds
#define tDB 5 // Device busy time in microseconds
// Note: tWC is in milliseconds, not microseconds like the rest of these timing parameters.
#define tWC 2 // Write cycle time in milliseconds.

// This will potentially be overidable in software. This default value if for a 28C64 EEPROM (8KB)
#define MAX_ADDRESS 0x2000 // Max address default

// Debug parameters
#define DEBUG_DATA 0xA9 // Debug data value to write during address test loop
#define DEBUG_DELAY 250 // Delay in milliseconds for debugging loops to allow visual verification of blinkenlights
#define ACTIVITY_LED 13 // Onboard LED pin is used as an activity indicator

// Serial comms parameters
#define SERIAL_BAUD 115200 // Baud rate for serial communication with Python control script
#define SERIAL_TIMEOUT 1000 // Serial timeout in milliseconds
// Use caution when adjusting MAX_PAYLOAD, as the Arduino has a serial buffer limit of 64 bytes, and you 
// will need to account for the serial protocol overhead. Max payload is calculated on a total of 16 S Records, 
// which is made up as per documentation.
#define MAX_PAYLOAD 24 // Maximum payload size for serial commands. 

#endif // DEFINES_H
