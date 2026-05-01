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

/**
 * @brief Initialize the EEPROM control and data pins
 * @details This function will set the pin modes for the EEPROM control lines (/WE, /OE, /CE) and the data 
 *          bus pins (D0-D7). 
 *          It will also set the control lines to their default inactive states (active low, so set to HIGH) 
 *          and clear the data bus.
 */
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

/**
 * @brief Set the current mode of the EEPROM (READ, WRITE, or STANDBY)
 * @param mode The desired mode to set the EEPROM to (READ, WRITE, or STANDBY)
 * @details This function will configure the control lines and data bus pins based on the desired mode. 
 *          For WRITE mode, it will set the data bus pins as outputs and configure the control lines for 
 *          writing. 
 *          For READ mode, it will set the data bus pins as inputs and configure the control lines for reading. 
 *          For STANDBY mode, it will set the control lines to disable both reading and writing, effectively 
 *          putting the EEPROM in a low-power standby state. The function also checks the current mode to avoid 
 *          unnecessary pin reconfigurations if the EEPROM is already in the desired mode.
 */
void setMode(ROMMode mode) { 

    setAddress(0x0000); // Clear the address lines when changing modes to avoid unintended writes or reads during mode transitions
    
    switch (mode)
    {
    case WRITE:
        digitalWrite(ROM_CE, LOW);
        digitalWrite(ROM_OE, HIGH);
        digitalWrite(ROM_WE, HIGH);

        setDataDirection(OUTPUT);
        
        delayMicroseconds(GEN_DELAY);
        currentMode = WRITE;
        delayMicroseconds(GEN_DELAY);
        break;

    case READ:
        if (currentMode != READ) {
            setDataDirection(INPUT);

            digitalWrite(ROM_CE, LOW);
            digitalWrite(ROM_OE, LOW);
            digitalWrite(ROM_WE, HIGH);

            delayMicroseconds(GEN_DELAY);
            currentMode = READ;
        }
        break;

    case STANDBY: // Default to standby mode
        setDataBus(0x00); // Clear the data bus when going to standby
        setDataDirection(INPUT); // Set data bus to input to avoid driving the lines

        digitalWrite(ROM_OE, LOW);
        digitalWrite(ROM_CE, HIGH);
        digitalWrite(ROM_WE, HIGH);

        delayMicroseconds(GEN_DELAY);
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
        digitalWrite(D0 + i, (data >> i) & 1);
    }  

}

/**
 * @brief Read the current value on the data bus pins
 * @return The 8-bit value currently present on the data bus pins (D0-D7)
 * @details This function will read the state of the data bus pins (D0-D7) and construct an 8-bit value 
 *          based on their states. It will iterate through each data pin, read its state (HIGH or LOW), and set 
 *          the corresponding bit in the returned byte accordingly.
 */
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
 * @details This function will set the address lines using the shift register, set the data bus to the 
 *          desired value, and then perform the necessary control line manipulations to execute a write 
 *          operation to the EEPROM. 
 *          After writing, it will return the value read back from the same address to verify that the 
 *          write was successful.
 */
uint8_t writeByte(uint16_t address, byte data) {


    // TODO: Uncomment this block when testing works. Or don't depending on outcome of testing.

    // setAddress(address);
    // setMode(WRITE);
    
    // setDataBus(data);
    // delayMicroseconds(DATA_DELAY);

    // digitalWrite(ROM_WE, LOW);
    // delayMicroseconds(WRITE_DELAY);
    // digitalWrite(ROM_WE, HIGH);
    // delayMicroseconds(WRITE_DELAY);

    // setMode(STANDBY);

    // TODO: Remove this code block once writing operations work reliably. Or implement it... Who knows which option will work...
    // Following the WE Controlled AC waveform diagram to the letter this time... 
    
    // Start by making sure we are in standby mode
    setMode(STANDBY);

    // Make sure the data lines are set to output
    setDataDirection(OUTPUT);
    setDataBus(0x00); // Clear the data bus before starting the write operation
    delayMicroseconds(GEN_DELAY);

    // TODO: Might be a good idea at this point to also define all the datasheet wait times as well and use those instead of arbitrary delays. 
    // This would make it easier to tweak the timing parameters as needed for reliable operation based on the specific EEPROM being programmed 
    // and the characteristics of the hardware setup. For now, we'll just use some arbitrary delays that are long enough to ensure reliable 
    // operation, but in the future it would be good to define these as constants based on the datasheet specifications.
    
    // This is all according to the timing diagram in the datasheet, with some added delays for good measure. 
    // The timing parameters can be tweaked as needed for reliable operation based on the specific EEPROM being 
    // programmed and the characteristics of the hardware setup.

    // First take the OE high to disable output and wait for tOES, which is a minimum of 10ns according to the datasheet, 
    // but we'll give it a bit more time, say 1us just to be safe.
    digitalWrite(ROM_OE, HIGH);
    delayMicroseconds(tOES);

    // Now set the address and wait tAS, which is also 10ns according to the datasheet, but again we'll give it a bit more time, say 1us just to be safe.
    setAddress(address);
    delayMicroseconds(tAS);

    // Next, take CE low to enable and wake up the chip
    digitalWrite(ROM_CE, LOW);
    delayMicroseconds(tCS);

    // Now take WE low. It seems weird that WE is taken low before setting the data, but this is what the timing 
    // diagram in the datasheet shows, so we'll follow it to the letter and see if it works. Here we need to now wait for tAH, which is a minimum of 50ns 
    // according to the datasheet, but we'll give it a bit more time, say 1us just to be safe. This is the time that WE needs to be held low before we can 
    // set the data.
    // In fact, if we read the datasheet correctly, it seems that the data should be set about halfway through tWP, which is 100-1000 ns according to the 
    // datasheet, so we can set the data after waiting for tAH, which is 50ns, and then wait for the remainder of tWP before taking WE high again.
    digitalWrite(ROM_WE, LOW);
    delayMicroseconds(tAH);

    // Now set the data bus and wait 
    setDataBus(data);
    delay(tDS);
    // Wait the required time as specified in tWP, which is the write pulse width, and is the time that WE needs 
    // to be held low for a successful write operation.
    // Actually, this is more like tDS, which we have already done.

    // Now take WE high and wait for tDH, which is the data hold time after WE goes high, and is a minimum of 50ns according to the datasheet, 
    // but we'll give it a bit more time, say 1us just to be safe.
    digitalWrite(ROM_WE, HIGH);
    delayMicroseconds(tDH);

    // One last delay, just to make sure the chip is done
    delay(tWC);

    // Now reset both busses back to defaults
    setDataBus(0x00);
    setMode(STANDBY);
    setDataDirection(INPUT);
    setAddress(0x0000);
    delayMicroseconds(GEN_DELAY);
    

    return readByte(address);

}

uint8_t writeTimingTest(uint16_t address, byte data) {

    setAddress(address);

    setMode(WRITE);

    // load data byte
    for (unsigned int i = 0; i < 8; i++) {
        digitalWrite(D0 + i, (data >> i) & 1);
    }
    delayMicroseconds(10);

    digitalWrite(ROM_WE, LOW);
    delayMicroseconds(10);
    digitalWrite(ROM_WE, HIGH);

    delayMicroseconds(10);
    setMode(STANDBY);

    return 0;

}

/**
 * @brief Read a byte of data from the EEPROM at the specified address
 * @param address The 16-bit address to read from
 * @return The 8-bit data value read from the EEPROM at the specified address
 * @details This function will set the address lines using the shift register, configure the control 
 *          lines for reading, and then read the value present on the data bus to return it as the 
 *          result of the read operation.
 */
uint8_t readByte(uint16_t address) {
    // byte data = 0;

    // setAddress(address);
    // delayMicroseconds(ADDR_DELAY);

    // setMode(READ);
    // delayMicroseconds(GEN_DELAY);
    // digitalWrite(ROM_CE, LOW);
    // delayMicroseconds(READ_DELAY);
    // digitalWrite(ROM_OE, LOW);
    // delayMicroseconds(READ_DELAY);

    // for (int pin = D0; pin <= D7; pin++) {
    //     data |= (digitalRead(pin) << pin);
    // }

    // setMode(STANDBY);

    setMode(READ);
    setAddress(address);
    delayMicroseconds(10);

    byte val = 0;
    for (unsigned int i = 0; i < 8; i++) {
        val |= (digitalRead(D0 + i) << i);
    }
    setMode(STANDBY);
    

    return val;
}

