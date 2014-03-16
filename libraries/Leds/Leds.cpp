#include "Leds.h"

Leds::Leds(short pins[8]) :
  ledInput(false),
  ledControl(pins)
{
}

void Leds::setLEDPattern(short row, short column) {
  flipLEDs(false, false);
  setLED(false, row);
  setLED(true, column);
  flipLEDs();
}

void Leds::lightWhosTurn(short whosTurn) {
  setInput(true, false);
  if (whosTurn > 0) {
    setLED(true, 0);
  } else {
    setLED(true, 7);
  }
  setInput(true, true);
}

void Leds::turnOff() {
  flipLEDs(false, false);
}

void Leds::setLED(boolean isColumn, short index) {
  short i = 0;
  if (isColumn) {
    i = 3;
  }
  digitalWrite(ledControl[0+i], byte(7 - index) & byte(1));
  digitalWrite(ledControl[1+i], (byte(7 - index) >> 1) & byte(1));
  digitalWrite(ledControl[2+i], (byte(7 - index) >> 2) & byte(1));
}

boolean Leds::flipLEDs(boolean flip, boolean input) {
  if (flip) {
    ledInput = !ledInput;
  } else {
    ledInput = input;
  }
  digitalWrite(ledControl[6], ledInput);
  digitalWrite(ledControl[7], ledInput);
  
  return ledInput;
}

void Leds::setInput(boolean isColumn, boolean input) {
  if (isColumn) {
    digitalWrite(ledControl[7], input);
  } else {
    digitalWrite(ledControl[6], input);
  }
}