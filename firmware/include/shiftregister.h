/** 
 * @file shiftregister.h
 * @brief Shift register control logic for 28Cx EEPROM programmer
 * @details Contains functions for setting up and controlling the shift register.
 *          This includes pin initialization and any low-level control functions needed to interface with the shift register.
 */

#ifndef SHIFTREGISTER_H
#define SHIFTREGISTER_H

#include <Arduino.h>

void setupShiftRegisterPins();
void clearShiftRegister();
void setAddress(uint16_t address);

#endif // SHIFTREGISTER_H
