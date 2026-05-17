/**
 * @file eeprom.h
 * @brief EEPROM control logic for 28Cx EEPROM programmer
 * @details Contains functions for setting up and controlling the EEPROM itself.
 *          This includes pin initialization and any low-level control functions needed to interface with the EEPROM.
 */


#ifndef EEPROM_H
#define EEPROM_H

#include <Arduino.h>

/**
 * @brief Enumeration for the different modes the EEPROM can be in (READ, WRITE, STANDBY)
 * @details This enum is used to track the current mode of the EEPROM and to configure the control lines and data bus pins accordingly when changing modes. 
 *          The modes are:
 *          - STANDBY: The EEPROM is in a low-power standby state, with control lines configured to disable both reading and writing.
 *          - READ: The EEPROM is configured for reading, with control lines set to enable reading and data bus pins set as inputs.
 *          - WRITE: The EEPROM is configured for writing, with control lines set to enable writing and data bus pins set as outputs.
 */
enum ROMMode { // Current EEPROM mode
    STANDBY,
    READ,
    WRITE
};

/**
 * @brief Global variable to track the current mode of the EEPROM (READ, WRITE, or STANDBY)
 * @details This variable is used to keep track of the current mode of the EEPROM so that the setMode function can 
 *          avoid unnecessary pin reconfigurations if the EEPROM is already in the desired mode.
 */
extern ROMMode currentMode;

/**
 * @brief Initialize the EEPROM control and data pins
 * @details This function will set the pin modes for the EEPROM control lines (/WE, /OE, /CE) and the data 
 *          bus pins (D0-D7). 
 *          It will also set the control lines to their default inactive states (active low, so set to HIGH) 
 *          and clear the data bus.
 */
void setupEEPROMPins();

/**
 * @brief Set the current mode of the EEPROM (READ, WRITE, or STANDBY)
 * @param mode The desired mode to set the EEPROM to (See enum ROMMode for details).
 * @details This function will configure the control lines and data bus pins based on the desired mode. 
 *          For WRITE mode, it will set the data bus pins as outputs and configure the control lines for 
 *          writing. 
 * 
 *          For READ mode, it will set the data bus pins as inputs and configure the control lines for reading. 
 *          For STANDBY mode, it will set the control lines to disable both reading and writing, effectively 
 *          putting the EEPROM in a low-power standby state. The function also checks the current mode to avoid 
 *          unnecessary pin reconfigurations if the EEPROM is already in the desired mode.
 */
void setMode(ROMMode mode);

/**
* @brief Set the data bus pins to the specified data value
* @param data The 8-bit data value to set on the data bus
* @details This function will set the data bus pins (D0-D7) to the corresponding bits of the data value. It will 
*          iterate through each bit of the data byte and set the corresponding data pin HIGH or LOW based on the 
*          value of that bit.
*/
void setDataBus(uint8_t data);

/**
 * @brief Read the current value on the data bus pins
 * @return The 8-bit value currently present on the data bus pins (D0-D7)
 * @details This function will read the state of the data bus pins (D0-D7) and construct an 8-bit value 
 *          based on their states. It will iterate through each data pin, read its state (HIGH or LOW), and set 
 *          the corresponding bit in the returned byte accordingly.
 */
uint8_t writeByte(uint16_t address, uint8_t data);

/**
 * @brief Write a byte of data to the EEPROM at the specified address
 * @param address The 16-bit address to write to
 * @param data The 8-bit data value to write to the EEPROM
 * @details This function will set the address lines using the shift register, set the data bus to the 
 *          desired value, and then perform the necessary control line manipulations to execute a write 
 *          operation to the EEPROM. 
 *          After writing, it will return the value read back from the same address to verify that the 
 *          write was successful.
 */
uint8_t readByte(uint16_t address);

/**
 * @brief Read a byte of data from the EEPROM at the specified address
 * @param address The 16-bit address to read from
 * @return The 8-bit data value read from the EEPROM at the specified address
 * @details This function will set the address lines using the shift register, configure the control 
 *          lines for reading, and then read the value present on the data bus to return it as the 
 *          result of the read operation.
 */
uint8_t writeTimingTest(uint16_t address, uint8_t data);

/**
 * @brief Set the data bus direction (input or output)
 * @param direction The desired direction for the data bus pins (INPUT or OUTPUT - default Arduino constants)
 * @details This function will set the pin modes for the data bus pins (D0-D7) based on the specified direction. 
 * If the direction is OUTPUT, the pins will be configured as outputs to allow writing data to the EEPROM. If the 
 * direction is INPUT, the pins will be configured as inputs to allow reading data from the EEPROM.
 */
void setDataDirection(uint8_t direction);

#endif // EEPROM_H