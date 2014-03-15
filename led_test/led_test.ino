#include <Leds.h>

boolean ledInput;
short ledControl[7] = {
  46,45,44,49,48,47,7
}; //A-C for row, A-C for column, input voltage

Leds leds(ledControl);
void setup() {
  Serial.begin(9600);
  //turn off LEDs if applicable
  for(int i = 0; i < 7; i++) {
    pinMode(ledControl[i], OUTPUT);
    digitalWrite(ledControl[i],LOW);
  }
  ledInput = false;
}

void loop() {
  for (short i = 0; i < 8; i++) {
    delay(500);
    leds.setLEDPattern(i, i);
  }
}
