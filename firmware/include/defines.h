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
/** EEPROM /WE (Write Enable) pin, defaults to Arduino pin D14 (A0) */
#define ROM_WE 14
/** EEPROM /OE (Output Enable) pin, defaults to Arduino pin D15 (A1) */
#define ROM_OE 15
/** EEPROM /CE (Chip Enable) pin, defaults to Arduino pin D16 (A2) */
#define ROM_CE 16

// EEPROM Data Pins
/** EEPROM data bus pin D0, defaults to Arduino pin D2 */
#define D0 2
/** EEPROM data bus pin D7, defaults to Arduino pin D9 */
#define D7 9

// 74HC595 Shift Register Control Pins
/** 74HC595 shift register clock pin, defaults to Arduino pin D12 */
#define SRCLK 12
/** 74HC595 shift register clear pin, defaults to Arduino pin D17 (A3) */
#define SRCLR 17
/** 74HC595 shift register latch pin, defaults to Arduino pin D10 */
#define RCLK 10
/** 74HC595 shift register output enable pin, defaults to Arduino pin D18 (A4) */
#define SEROE 18
/** 74HC595 shift register data pin, defaults to Arduino pin D11 */
#define DATA 11

// Timing Parameters
#define WRITE_DELAY 10 // Delay in microseconds for programming operations
#define READ_DELAY 1 // Delay in microseconds for read operations
#define ADDR_DELAY 1 // Delay in microseconds for address setup
#define DATA_DELAY 1 // Delay in microseconds for data setup
#define GEN_DELAY 10 // General delay in microseconds

// More timing parameters
/** Address setup time in microseconds */
#define tAS 1
/** Output enable setup time in microseconds */
#define tOES 1
/** Address hold time in microseconds */
#define tAH 5
/** Write pulse width in microseconds */
#define tWP 10
/** Data setup time in microseconds */
#define tDS 5
/** Data hold time in microseconds */
#define tDH 5
/** Output enable hold time in microseconds */
#define tOEH 1
/** Chip select setup time in microseconds */
#define tCS 1
/** Chip select hold time in microseconds */
#define tCH 1
/** Device busy time in microseconds */
#define tDB 5
/** Write cycle time in milliseconds. Note: This value is in milliseconds, not microseconds like the rest of these timing parameters.
*/
#define tWC 2

/** Max address default. This will potentially be overidable in software. This default value if for a 28C64 EEPROM (8KB) */
#define MAX_ADDRESS 0x2000

// Debug parameters
/** Debug data value to write during address test loop */
#define DEBUG_DATA 0xA9
/** Delay in milliseconds for debugging loops to allow visual verification of blinkenlights */
#define DEBUG_DELAY 250

// Serial comms parameters
/** Baud rate for serial communication with Python control script */
#define SERIAL_BAUD 115200

#endif // DEFINES_H
