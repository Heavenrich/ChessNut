#include "Leds.h"

Leds::Leds(short pins[7]) :
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
  flipLEDs(false, false);
  if (whosTurn > 0) {
    setLED(true, 0);
  } else {
    setLED(true, 7);
  }
}

void Leds::setLED(boolean isColumn, short index) {
  int i = 0;
  if (isColumn) {
    i = 3;
  }
  digitalWrite(ledControl[2+i], byte(7-index) & byte(1));
  digitalWrite(ledControl[1+i], (byte(7-index) >> 1) & byte(1));
  digitalWrite(ledControl[0+i], (byte(7-index) >> 2) & byte(1));
}

boolean Leds::flipLEDs(boolean flip, boolean input) {
  if (flip) {
    ledInput = !ledInput;
  } else {
    ledInput = input;
  }
  digitalWrite(ledControl[6], ledInput);
  
  return ledInput;
}