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

ROMMode currentMode = ROMMode::STANDBY; // Global variable to track the current mode of the EEPROM (read/write/standby)

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
    delayMicroseconds(GEN_DELAY);

}   

void setDataDirection(uint8_t direction) { // true for output, false for input

    for (uint8_t i = 0; i < 8; i++) {
        pinMode(D0 + i, direction);
    }

}

void setMode(ROMMode mode) { 

    setAddress(0x0000); // Clear the address lines when changing modes to avoid unintended writes or reads during mode transitions
    
    switch (mode)
    {
    case ROMMode::WRITE:
        digitalWrite(ROM_CE, LOW);
        digitalWrite(ROM_OE, HIGH);
        digitalWrite(ROM_WE, HIGH);

        setDataDirection(OUTPUT);
        
        delayMicroseconds(GEN_DELAY);
        currentMode = ROMMode::WRITE;
        delayMicroseconds(GEN_DELAY);
        break;

    case ROMMode::READ:
        if (currentMode != ROMMode::READ) {
            setDataDirection(INPUT);

            digitalWrite(ROM_CE, LOW);
            digitalWrite(ROM_OE, LOW);
            digitalWrite(ROM_WE, HIGH);

            delayMicroseconds(GEN_DELAY);
            currentMode = ROMMode::READ;
        }
        break;

    case ROMMode::STANDBY: // Default to standby mode
        setDataBus(0x00); // Clear the data bus when going to standby
        setDataDirection(INPUT); // Set data bus to input to avoid driving the lines

        digitalWrite(ROM_OE, LOW);
        digitalWrite(ROM_CE, HIGH);
        digitalWrite(ROM_WE, HIGH);

        delayMicroseconds(GEN_DELAY);
        currentMode = ROMMode::STANDBY;

        break;
        
    }

}

void setDataBus(uint8_t data) {

    for (int i = 0; i < 8; i++) {
        digitalWrite(D0 + i, (data >> i) & 1);
    }  

}

uint8_t readBus() {
    uint8_t data = 0;
    for (int i = 0; i < 8; i++) {
        data |= (digitalRead(D0 + i) << i);
    }
    return data;
}

uint8_t writeByte(uint16_t address, byte data) {
    // Excuse the verbosity of the comments in this method, the timing on this is very sensitive and complex.
    
    // Start by making sure we are in standby mode
    setMode(ROMMode::STANDBY);

    // Make sure the data lines are set to output
    setDataDirection(OUTPUT);
    setDataBus(0x00); // Clear the data bus before starting the write operation
    delayMicroseconds(GEN_DELAY);

    digitalWrite(ROM_OE, HIGH);
    delayMicroseconds(tOES);

    // Now set the address and wait tAS
    setAddress(address);
    delayMicroseconds(tAS);

    // Next, take CE low to enable and wake up the chip
    digitalWrite(ROM_CE, LOW);
    delayMicroseconds(tCS);

    // Now take WE low
    // In fact, if we read the datasheet correctly, it seems that the data should be set about halfway through tWP, which is 100-1000 ns according to the 
    // datasheet, so we can set the data after waiting for tAH, which is 50ns, and then wait for the remainder of tWP before taking WE high again.
    digitalWrite(ROM_WE, LOW);
    delayMicroseconds(tAH);

    // Now set the data bus and wait 
    setDataBus(data);
    delayMicroseconds(tDS);

    digitalWrite(ROM_WE, HIGH);
    delayMicroseconds(tDH);

    // One last delay, just to make sure the chip is done
    delayMicroseconds(GEN_DELAY);

    // Now reset both busses back to defaults
    setDataBus(0x00);
    setMode(ROMMode::STANDBY);
    setDataDirection(INPUT);
    setAddress(0x0000);
    delayMicroseconds(GEN_DELAY);
    

    return readByte(address);

}

uint8_t readByte(uint16_t address) {
    
    setMode(ROMMode::READ);
    setAddress(address);
    delayMicroseconds(10);

    byte val = 0;
    for (unsigned int i = 0; i < 8; i++) {
        val |= (digitalRead(D0 + i) << i);
    }
    setMode(ROMMode::STANDBY);
    

    return val;
}

