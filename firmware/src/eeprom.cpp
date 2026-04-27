/**
 * EEPROM Programmer Firmware
 * @file eeprom.cpp
 * @brief Functions for controlling 28Cx EEPROM programming and reading
 * @details Contains functions for setting up and controlling the EEPROM itself.
 *          This includes pin initialization and any low-level control functions needed to interface with the EEPROM.
 */

#include <Arduino.h>
#include "defines.h"
#include "shiftregister.h"
#include "eeprom.h"

void setupEEPROMPins() {

    // Setup control pins as outputs
    if (DEBUG_MODE) {
        Serial.println("Setting up EEPROM control pins...");
    }
    pinMode(ROM_WE, OUTPUT);
    pinMode(ROM_OE, OUTPUT);
    pinMode(ROM_CE, OUTPUT);

    // Set control lines to inactive state. These are active low, so setting to HIGH means inactive.
    if (DEBUG_MODE) {
        Serial.println("Setting EEPROM control lines to default state...");
    }
    digitalWrite(ROM_WE, HIGH); // /WE inactive, so cannot write until this line is taken low
    digitalWrite(ROM_OE, LOW); // /OE defaults to low, as this is active low, and indicates a read cycle
    digitalWrite(ROM_CE, HIGH); // /CE inactive and chip not selected

    // Setup data bus pins as outputs
    setDataBus(0x00); // Clear the data bus on startup
    delayMicroseconds(GENDELAY);

}   

/**
* @brief Set the data bus pins to the specified data value
* @param data The 8-bit data value to set on the data bus
* @details This function will set the data bus pins (D0-D7) to the corresponding bits of the data value. It will 
*          iterate through each bit of the data byte and set the corresponding data pin HIGH or LOW based on the 
*          value of that bit.
*/
void setDataBus(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        uint8_t bitValue = (data >> i) & 0x01; // Extract the i-th bit
        digitalWrite(D7 - i, bitValue); // Set the corresponding data pin
    }
    if (DEBUG_MODE) {
        delay(DEBUG_DELAY);
    }
    else {
        delayMicroseconds(DATADELAY);
    }
    
}

/**
 * @brief Write a byte of data to the EEPROM at the specified address
 * @param address The 16-bit address to write to
 * @param data The 8-bit data value to write to the EEPROM
 * @details This function will set the address lines using the shift register, 
 *          set the data bus to the desired value, and then perform the necessary 
 *          control line manipulations to execute a write operation to the EEPROM. 
 */
void writeByte(uint16_t address, uint8_t data) {
    // Set the address lines using the shift register
    setAddress(address);

    // Set the data bus to the desired value
    setDataBus(data);

    // Perform the write operation to the EEPROM
    doWrite();

    delay(DEBUG_DELAY);

    setDataBus(0x00); // Clear the data bus
}

/**
* @brief Perform a write operation to the EEPROM at the currently set address with the data on the data bus
* @details This function will execute the necessary control line manipulations to perform a write operation 
*          to the EEPROM. It assumes that the address has already been set using the shift register and that 
*          the data bus has been set to the desired value using setDataBus(). The function will pulse the /WE 
*          line low for the required duration to write the data to the EEPROM, and then return the control lines 
*          to their inactive states.
*/
void doWrite() {
    
    // Set /OE high to indicate a write cycle is about to start
    if (DEBUG_MODE) {
        Serial.println("Starting EEPROM write cycle...");
        Serial.println("Setting /OE high...");
    }
    digitalWrite(ROM_OE, HIGH);

    // Now pulse /WE low to write the data
    if (DEBUG_MODE) {
        Serial.println("Doing write...");
    }
    digitalWrite(ROM_WE, LOW); // Set low to initiate the write
    if (DEBUG_MODE) {
        delay(DEBUG_DELAY);
    }
    else {
        delayMicroseconds(PROGDELAY);
    }
    digitalWrite(ROM_WE, HIGH);

    // Set /OE high to disable writing
    if (DEBUG_MODE) {
        Serial.println("Ending EEPROM write cycle...");
        Serial.println("Setting /OE high...");
    }
    digitalWrite(ROM_OE, LOW); //Returns to low to end the write and return to the default read state
    if (DEBUG_MODE) {
        delay(DEBUG_DELAY);
    }
    else {
        delayMicroseconds(PROGDELAY);
    }

    // TODO: Add data polling support to confirm successful write

}
