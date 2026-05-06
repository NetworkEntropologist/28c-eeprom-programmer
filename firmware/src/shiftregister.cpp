/**
 * EEPROM Programmer Firmware
 * @file shiftregister.cpp
 * @brief Functions for controlling 74HC595 shift register for 28Cx EEPROM programmer
 * @details Contains functions for setting up and controlling the shift register itself.
 *          This includes pin initialization and any low-level control functions needed to interface with the shift register.
 */

#include "shiftregister.h"
#include "defines.h"

void setupShiftRegisterPins() {

    // Set shift register control pins as outputs
    pinMode(SRCLK, OUTPUT);
    pinMode(SRCLR, OUTPUT);
    pinMode(RCLK, OUTPUT);
    pinMode(SEROE, OUTPUT);
    pinMode(DATA, OUTPUT);

    // Initialize control pins to default states
    digitalWrite(SRCLK, LOW);
    digitalWrite(SRCLR, HIGH); // Not clearing the shift register
    digitalWrite(RCLK, LOW);
    digitalWrite(SEROE, LOW); // Enable output
    digitalWrite(DATA, LOW);

    delayMicroseconds(ADDR_DELAY);

    setAddress(0x0000); // Clear the address lines

}

void setAddress(uint16_t address) {
    // Shift out the address to the shift registers.

    // TODO: Add error handling to check the address is in range.

    // Disable output
    digitalWrite(SEROE, HIGH);
        
    // Disable latch
    digitalWrite(RCLK, LOW);
                
    // Shift out the new address (MSB first)
    shiftOut(DATA, SRCLK, MSBFIRST, address >> 8); // Upper byte
    shiftOut(DATA, SRCLK, MSBFIRST, address); // Lower byte
        
    // Latch the new address
    digitalWrite(RCLK, HIGH);
    digitalWrite(SEROE, LOW);
        
}
