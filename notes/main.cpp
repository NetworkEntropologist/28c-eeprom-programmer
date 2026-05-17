#include <Arduino.h>

#define LED_PIN 13
#define MAX_PAYLOAD 16 // Max serial payload - setting to 16 bytes for development

int inByte = 0;        // incoming serial byte
char inChar = ' ';     // incoming serial char
bool connected = false;

void setup() {
  // put your setup code here, to run once:

  pinMode(LED_PIN, OUTPUT);  // initialize the LED pin as an output
  digitalWrite(LED_PIN, LOW); // turn the LED off by making the voltage LOW

  // start serial port at 115200 bps:
  Serial.begin(115200);
  Serial.setTimeout(120000L); // set the timeout to 2 minutes
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

}

void loop() {

  if (Serial.available() > 0) {
    // byte buf[1 + MAX_PAYLOAD]; // buffer for incoming serial data]
    // digitalWrite(LED_PIN, HIGH); // Turn on the LED while processing serial data

    // const int len = receive(buf, sizeof(buf), false);
    
    // if (len > 0) {
    //   // For starters, let's just echo the received data back
    //   send(buf, len, false);
    // }

    // For development, just read the incoming byte and echo it back
    inByte = Serial.read();
    inChar = (char)inByte;
    Serial.print(inChar);

    digitalWrite(LED_PIN, LOW); // Turn off the LED after processing serial data

}

}

