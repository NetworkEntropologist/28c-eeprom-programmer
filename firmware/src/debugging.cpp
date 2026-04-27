/**
* EEPROM Programmer Firmware
* @file debugging.cpp
* @brief Debugging utilities for 28Cx EEPROM programmer firmware
* @details Contains functions for debugging and testing the EEPROM programmer firmware.
*          This may include functions for testing pin states, timing, and communication with the Python control script.
*          These functions can be called from the main loop or setup to verify correct operation of the hardware and firmware.
*/

#include <Arduino.h>
#include "defines.h"
#include "debugging.h"
#include "eeprom.h"
#include "shiftregister.h"

/**
* @brief Test function to loop through all addresses and set the address lines and data bus for each address
* @details This function is intended for debugging purposes. It will iterate through all possible addresses,
*          set the address lines accordingly, place a known data value on the data bus, and perform a programming cycle.
*          This allows verification of the hardware setup and the correct operation of the firmware.
*          The function can be called from the main loop or setup to visually verify the blinkenlights on the board while writing the firmware code.
* @note This function is for testing and debugging only. It should not be called during normal operation of the EEPROM programmer, as it will overwrite data on the EEPROM.
 */
void addressTestLoop() {
    // Loop through all addresses from 0 to MAX_ADDRESS and write a test value (e.g., 0xAA) to each address
    for (uint16_t address = 0; address <= MAX_ADDRESS; address++) {
        Serial.print("Testing address: 0x");
        Serial.println(address, HEX);
        writeByte(address, DEBUG_DATA);
        delay(DEBUG_DELAY);
    }
}
