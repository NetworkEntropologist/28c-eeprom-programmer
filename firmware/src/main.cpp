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
#include "wozmon.h"

void setup() {
  
  Serial.begin(SERIAL_BAUD);

  setupEEPROMPins();

  // Setup the shift register control pins
  setupShiftRegisterPins();

  //writeTest(0x00, 0x2000);
  //eraseTest(0x00, 0x2000);
  //delay(5000);
  //writeWozMon();
  quickReadTest(0x00, 0x2000);

}

void loop() {

  
  
}

