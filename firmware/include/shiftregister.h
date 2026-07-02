/** 
 * @file shiftregister.h
 * @brief Shift register control logic for 28Cx EEPROM programmer
 * @details Contains functions for setting up and controlling the shift register.
 *          This includes pin initialization and any low-level control functions needed to interface with the shift register.
 */

#ifndef SHIFTREGISTER_H
#define SHIFTREGISTER_H

#include <Arduino.h>

/**
* @brief Initialize all 74HC595 shift register control pins
* @details Sets all shift register pins to OUTPUT mode and initializes them to safe states.
*          Clears the shift register on startup to ensure a known state.
*          This function should be called once during system initialization.
* @note Must be called before any shift register operations
*/
void setupShiftRegisterPins();

/**
* @brief Set the address lines on the EEPROM using the shift register
* @param address The 16-bit address to set on the EEPROM
* @details This function shifts out the address to the 74HC595 shift register(s) to set the address lines on the EEPROM.
*          The implementation depends on the hardware configuration and how the shift registers are connected to the EEPROM address lines.
*/
void setAddress(uint16_t address);

#endif // SHIFTREGISTER_H
