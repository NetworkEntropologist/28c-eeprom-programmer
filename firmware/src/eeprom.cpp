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

ROMMode currentMode = STANDBY; // Global variable to track the current mode of the EEPROM (read/write/standby)

void setupEEPROMPins() {

    // Setup control pins as outputs
    pinMode(ROM_WE, OUTPUT);
    pinMode(ROM_OE, OUTPUT);
    pinMode(ROM_CE, OUTPUT);

    // Set control lines to inactive state. These are active low, so setting to HIGH means inactive.
    digitalWrite(ROM_WE, HIGH); // /WE inactive, so cannot write until this line is taken low
    digitalWrite(ROM_OE, HIGH); // /OE defaults to low, as this is active low, and indicates a read cycle
    digitalWrite(ROM_CE, LOW); // /CE inactive and chip not selected

    // Setup data bus pins as outputs
    setDataBus(0x00); // Clear the data bus on startup
    delayMicroseconds(GENDELAY);

}   

void setMode(ROMMode mode) { // True for write mode, false for read mode
    
    // TODO: Add logic to set mode correctly

    switch (mode)
    {
    case WRITE:
        if (currentMode != WRITE) {
            for (unsigned int i = 0; i < 8; i++) {
                pinMode(D0 + i, OUTPUT);
            }
        
            digitalWrite(ROM_CE, LOW);
            digitalWrite(ROM_OE, HIGH);
            digitalWrite(ROM_WE, HIGH);
        
            delayMicroseconds(GENDELAY);
            currentMode = WRITE;
        }
        break;
    case READ:
        if (currentMode != READ) {
            for (unsigned int i = 0; i < 8; i++) {
                pinMode(D0 + i, INPUT);
            }
        
            digitalWrite(ROM_CE, LOW);
            digitalWrite(ROM_OE, LOW);
            digitalWrite(ROM_WE, HIGH);
        
            delayMicroseconds(GENDELAY);
            currentMode = READ;
        }
        break;
    default: // Default to standby mode
        digitalWrite(ROM_OE, LOW);
        digitalWrite(ROM_CE, HIGH);
        digitalWrite(ROM_WE, HIGH);
        currentMode = STANDBY;
        break;
    }

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
        pinMode(D0 + i, OUTPUT);
        uint8_t bitValue = (data >> i) & 0x01;
        digitalWrite(D7 - i, bitValue);
    }  

}

uint8_t readBus() {
    uint8_t data = 0;
    for (int i = 0; i < 8; i++) {
        data |= (digitalRead(D0 + i) << i);
    }
    return data;
}

/**
 * @brief Write a byte of data to the EEPROM at the specified address
 * @param address The 16-bit address to write to
 * @param data The 8-bit data value to write to the EEPROM
 * @details This function will set the address lines using the shift register, 
 *          set the data bus to the desired value, and then perform the necessary 
 *          control line manipulations to execute a write operation to the EEPROM. 
 */
uint8_t writeByte(uint16_t address, byte data) {

    setAddress(address);
    setMode(WRITE);
    setDataBus(data);
    delayMicroseconds(DATADELAY);

    digitalWrite(ROM_WE, LOW);
    delayMicroseconds(GENDELAY);
    digitalWrite(ROM_WE, HIGH);
    delayMicroseconds(GENDELAY);

    setMode(STANDBY);

    return readByte(address);

}

uint8_t readByte(uint16_t address) {
    byte data = 0;

    for (int pin = D0; pin <= D7; pin++) {
        pinMode(pin, INPUT);
    }

    // digitalWrite(ROM_CE, HIGH);
    digitalWrite(ROM_OE, HIGH);
    setAddress(address);
    digitalWrite(ROM_CE, LOW);
    digitalWrite(ROM_OE, LOW);

    for (int pin = D0; pin <= D7; pin++) {
        data = (data << 1) | digitalRead(pin);
    }

    digitalWrite(ROM_OE, HIGH);
    // digitalWrite(ROM_CE, HIGH);

    return data;
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
    
    // setMode(true); // Ensure write mode is set

    // digitalWrite(ROM_WE, LOW); // Set low to initiate the write
    // delayMicroseconds(PROGDELAY);
    // delay(DEBUG_DELAY);
    // digitalWrite(ROM_WE, HIGH);

    // setMode(false); // Return to read mode

}
