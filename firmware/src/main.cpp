/**
 * @file main.cpp
 * @brief Arduino Nano firmware for 28Cx EEPROM programmer
 * @author The Network Entropologist
 * @version 0.1
 * @details Supports programming of 28C16, 28C64, 28C256, and 28C512 EEPROMs.
 *          Hardware: Arduino Nano with 74HC595 shift registers.
 *          Driven by Python control script.
 *          Inspired by https://github.com/beneater/eeprom-programmer
 *          Pin assignments and timing parameters are defined in include/defines.h.
 *          Update pin assignments based on your own hardware design.
 *          Update timing parameters as needed for reliable programming.
 *          Best to consult the datasheet of the specific EEPROM being programmed for timing requirements.
 *          Suggest 100us as a starting point for programming timing.
 * @license MIT License (See LICENSE-MIT file in project root)
 */

#include <Arduino.h>
#include "defines.h"
#include "eeprom.h"
#include "shiftregister.h"
#include "debugging.h"

void testing() {

  delay(DEBUG_DELAY);
  
  for (uint8_t addr = 0; addr <= 0xf; addr++) {

    // uint8_t data = readByte(addr);
    // Serial.print("A0x");
    // Serial.print(addr, HEX);
    // Serial.print(" reads as: 0x");
    // Serial.println(data, HEX);
    
    uint8_t data_written = writeByte(addr, DEBUG_DATA);
    Serial.print("Address 0x");
    Serial.print(addr, HEX);
    Serial.print(" should be : 0x");
    Serial.print(DEBUG_DATA, HEX);
    Serial.print(" and reads back as: 0x");
    Serial.println(data_written, HEX);

    // Serial.print("Address: 0x");
    // Serial.print(addr, HEX);
    // Serial.print(" is 0x");
    // Serial.println(readByte(addr), HEX);
    // delay(DEBUG_DELAY);

  }

}

void setup() {
  
  Serial.begin(SERIAL_BAUD);

  setupEEPROMPins();

  // Setup the shift register control pins
  setupShiftRegisterPins();


  testing();

}

void loop() {

  
  
}

