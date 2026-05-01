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

void addressWalkRead() {
    // Loop through all addresses from 0 to MAX_ADDRESS and read the value at each address, printing it to the serial monitor
    for (uint16_t address = 0; address <= MAX_ADDRESS; address++) {
        uint8_t data = readByte(address);
        Serial.print("Address 0x");
        Serial.print(address, HEX);
        Serial.print(": 0x");
        Serial.println(data, HEX);
        delay(DEBUG_DELAY);
    }
}

void quickReadTest(uint16_t startAddress, uint16_t endAddress) {

    byte data_line[15];
    uint16_t line_start = 0x0000;
    uint8_t index = 0;

    for (uint16_t address = startAddress; address <= endAddress; address++){

        if (index == 0) {
            line_start = address;
        }

        uint8_t data = readByte(address);
        
        data_line[index] = data;
        index++;

        if (index >= 15 or address == endAddress) {

             // Format the address
            char addr_buffer[5];
            sprintf(addr_buffer, "%04X", line_start);
            Serial.print(addr_buffer);
            Serial.print("  ");

            for (uint8_t i = 0; i < index; i++) {
                char data_buffer[3];
                sprintf(data_buffer, "%02X", data_line[i]);

                Serial.print(data_buffer);
                if (i == 7) {
                    Serial.print("  ");
                }
                else {
                    Serial.print(" ");
                }
            }

            for (uint8_t i = 0; i < index; i++) {
                if (data_line[i] < 32 or data_line[i] > 126) {
                    Serial.print(".");
                }
                else {
                    Serial.print((char)data_line[i]);
                }
            }
            Serial.println();
            index = 0;
        }

    }

}

void writeTest(uint16_t startAddress, uint16_t endAddress) {

    for (uint16_t address = startAddress; address <= endAddress; address++) {
        
        Serial.print("Writing to address 0x");
        Serial.print(address, HEX);
        Serial.print(" value 0x");
        Serial.println(DEBUG_DATA, HEX);
        writeByte(address, DEBUG_DATA); // Write the lower byte of the address as test data
        //writeTimingTest(address, DEBUG_DATA);
    }
}

void eraseTest(uint16_t startAddress, uint16_t endAddress) {

    for (uint16_t address = startAddress; address <= endAddress; address++) {
        
        Serial.print("Erasing address 0x");
        Serial.println(address, HEX);
        writeByte(address, 0x00); // Write 0x00 to erase the byte at the specified address
    }

}

