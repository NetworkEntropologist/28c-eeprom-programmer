/**
 * EEPROM Programmer Firmware
 * @file debugging.h
 * @brief Debugging utilities for 28Cx EEPROM programmer firmware
 * @details Contains functions for debugging and testing the EEPROM programmer firmware.
 *          This may include functions for testing pin states, timing, and communication with the Python control script.
 *          These functions can be called from the main loop or setup to verify correct operation of the hardware and firmware.
 */

#ifndef DEBUGGING_H
#define DEBUGGING_H

void addressTestLoop();
void addressWalkRead();
void quickReadTest(uint16_t startAddress, uint16_t endAddress);
void writeTest(uint16_t startAddress, uint16_t endAddress);
void eraseTest(uint16_t startAddress, uint16_t endAddress);

#endif // DEBUGGING_H