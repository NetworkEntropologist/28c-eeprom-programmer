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

  // These are some testing methods used during debugging. Leaving them here, as they are useful for debugging purposes.
  //writeTest(0xaa, 0x8);
  //eraseTest(0x00, 0x000f);
  //delay(5000);
  //writeWozMon();
  // Read the EEPROM from WOZMON to the end of the address space
  //quickReadTest(0x0000, 0x2000); 

}

void loop() {

  if (Serial.available() > 0) {
  
    String command = Serial.readStringUntil('\n');
    command.trim(); // Remove any leading/trailing whitespace

    if (command.equalsIgnoreCase("D")) {

      // Read the entire EEPROM and send the raw bytes back over serial
      for (uint16_t addr = 0; addr < 0x2000; addr++) {
        uint8_t data = readByte(addr);
        Serial.write(data);            
      }

    }
  
  }

}

