
#include <Arduino.h>
#include "wozmon.h"
#include "eeprom.h"

void writeWozMon() {

    for (uint16_t i = 0; i < sizeof(WOZMON_CODE); i++) {
        writeByte(WOZMON_START_ADDRESS + i, WOZMON_CODE[i]);
        Serial.print("Written byte 0x");
        Serial.print(WOZMON_CODE[i], HEX);
        Serial.print(" to address 0x");
        Serial.println(WOZMON_START_ADDRESS + i, HEX);
        delay(10); // Small delay to allow the chip to settle and cool down
    }
}
