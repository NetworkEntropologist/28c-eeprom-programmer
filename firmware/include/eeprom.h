/**
 * @file eeprom.h
 * @brief EEPROM control logic for 28Cx EEPROM programmer
 * @details Contains functions for setting up and controlling the EEPROM itself.
 *          This includes pin initialization and any low-level control functions needed to interface with the EEPROM.
 */


#ifndef EEPROM_H
#define EEPROM_H

void setupEEPROMPins();
void clearDataBus();
void setDataBus(uint8_t data);
void writeByte(uint16_t address, uint8_t data);
void doWrite();

#endif // EEPROM_H