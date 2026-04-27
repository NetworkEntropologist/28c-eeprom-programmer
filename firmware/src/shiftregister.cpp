/**
 * EEPROM Programmer Firmware
 * @file shiftregister.cpp
 * @brief Functions for controlling 74HC595 shift register for 28Cx EEPROM programmer
 * @details Contains functions for setting up and controlling the shift register itself.
 *          This includes pin initialization and any low-level control functions needed to interface with the shift register.
 */

#include "shiftregister.h"
#include "defines.h"

/**
* @brief Initialize all 74HC595 shift register control pins
* @details Sets all shift register pins to OUTPUT mode and initializes them to safe states.
*          Clears the shift register on startup to ensure a known state.
*          This function should be called once during system initialization.
* @note Must be called before any shift register operations
* @see clearShiftRegister()
*/
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

    if (DEBUG_MODE) {
        Serial.println("Shift register control pins initialized.");
    }
    delayMicroseconds(ADDRDELAY);

    setAddress(0x0000); // Clear the address lines

}

/**
* @brief Set the address lines on the EEPROM using the shift register
* @param address The 16-bit address to set on the EEPROM
* @details This function shifts out the address to the 74HC595 shift register(s) to set the address lines on the EEPROM.
*          The implementation depends on the hardware configuration and how the shift registers are connected to the EEPROM address lines.
*/
void setAddress(uint16_t address) {
    // Shift out the address to the shift registers.

    // Disable output
    if (DEBUG_MODE) {
        Serial.println("Disabling shift register output...");
    }
    digitalWrite(SEROE, HIGH); // Disable output while shifting new address
    delayMicroseconds(ADDRDELAY);
        
    // Disable latch
    if (DEBUG_MODE) {
        Serial.println("Disabling shift register latch...");
    }
    digitalWrite(RCLK, LOW); // Disable latch to prepare for shifting new address
    delayMicroseconds(ADDRDELAY);
            
    // Shift out the new address (MSB first)
    if (DEBUG_MODE) {
        Serial.print("Shifting out address: 0x");
        Serial.println(address, HEX);
    }
    shiftOut(DATA, SRCLK, MSBFIRST, address >> 8); // Upper byte
    shiftOut(DATA, SRCLK, MSBFIRST, address); // Lower byte
    delayMicroseconds(ADDRDELAY);
    
    // Latch the new address
    if (DEBUG_MODE) {
        Serial.println("Latching new address...");
    }
    digitalWrite(RCLK, HIGH); // Latch the new address
    delayMicroseconds(ADDRDELAY);
        
    // Re-enable output
    if (DEBUG_MODE) {
        Serial.println("Re-enabling shift register output...");
    }
    digitalWrite(SEROE, LOW); // Re-enable output to apply new address 
    delayMicroseconds(ADDRDELAY);
    
}