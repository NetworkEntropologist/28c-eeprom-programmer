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
#include "serialio.h"
#include "debugging.h"
#include "wozmon.h"

void setup() {
  
  Serial.begin(SERIAL_BAUD);
  Serial.setTimeout(SERIAL_TIMEOUT);
  pinMode(ACTIVITY_LED, OUTPUT);

  setupEEPROMPins();

  // Setup the shift register control pins
  setupShiftRegisterPins();

  // Clear the serial buffers
  memset(in_buffer, 0, sizeof(in_buffer));
  memset(out_buffer, 0, sizeof(out_buffer));

  //Serial.println("READY");

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
    digitalWrite(ACTIVITY_LED, HIGH); // LED on to indicate a serial command is being processed

    const int len = serialReceive(sizeof(in_buffer), true);

    if (len > 0) {
      int command_valid = parseCommand();
      if (command_valid != 0) { // Invalid command packet received
        sendErrorResponse();
      }
      else { // Valid command packet received
        processSerialCommand();
      }
    }

    // Clear the input buffer
    memset(in_buffer, 0, sizeof(in_buffer));
      
    // String command = Serial.readStringUntil('\n');
    // command.trim(); // Remove any leading/trailing whitespace

    digitalWrite(ACTIVITY_LED, LOW);
  
  }

}

