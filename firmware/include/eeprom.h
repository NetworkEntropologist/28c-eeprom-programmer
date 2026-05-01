/**
 * @file eeprom.h
 * @brief EEPROM control logic for 28Cx EEPROM programmer
 * @details Contains functions for setting up and controlling the EEPROM itself.
 *          This includes pin initialization and any low-level control functions needed to interface with the EEPROM.
 */


#ifndef EEPROM_H
#define EEPROM_H

#include <Arduino.h>

enum ROMMode { // Current EEPROM mode
    STANDBY,
    READ,
    WRITE
};

void setupEEPROMPins();
void setMode(ROMMode mode);
void setDataBus(uint8_t data);
uint8_t writeByte(uint16_t address, uint8_t data);
uint8_t readByte(uint16_t address);
uint8_t writeTimingTest(uint16_t address, uint8_t data);
void setDataDirection(uint8_t direction);

extern ROMMode currentMode;

#endif // EEPROM_H