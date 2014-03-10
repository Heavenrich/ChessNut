#include "Leds.h"

Leds::Leds(short pins[7]) :
  ledInput(false)
{
  for (short i = 0; i < 7; i++) {
    ledControl[i] = pins[i];
  }
}

void Leds::setLEDPattern(int row, int column) {
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

void Leds::setLED(boolean isColumn, int index) {
  int i = 0;
  if (i) {
    i = 3;
  }
  switch(index) {
     case 0:
       digitalWrite(ledControl[0+i],LOW);
       digitalWrite(ledControl[1+i],LOW);
       digitalWrite(ledControl[2+i],LOW);
       break;
    case 1:
       digitalWrite(ledControl[0+i],HIGH);
       digitalWrite(ledControl[1+i],LOW);
       digitalWrite(ledControl[2+i],LOW);
       break;
    case 2:
       digitalWrite(ledControl[0+i],LOW);
       digitalWrite(ledControl[1+i],HIGH);
       digitalWrite(ledControl[2+i],LOW);
       break;
    case 3:
       digitalWrite(ledControl[0+i],HIGH);
       digitalWrite(ledControl[1+i],HIGH);
       digitalWrite(ledControl[2+i],LOW);
       break;
    case 4:
       digitalWrite(ledControl[0+i],LOW);
       digitalWrite(ledControl[1+i],LOW);
       digitalWrite(ledControl[2+i],HIGH);
       break;
    case 5:
       digitalWrite(ledControl[0+i],HIGH);
       digitalWrite(ledControl[1+i],LOW);
       digitalWrite(ledControl[2+i],HIGH);
       break;
    case 6:
       digitalWrite(ledControl[0+i],LOW);
       digitalWrite(ledControl[1+i],HIGH);
       digitalWrite(ledControl[2+i],HIGH);
       break;
    case 7:
       digitalWrite(ledControl[0+i],HIGH);
       digitalWrite(ledControl[1+i],HIGH);
       digitalWrite(ledControl[2+i],HIGH);
       break;
     default:
       digitalWrite(ledControl[0+i],LOW);
       digitalWrite(ledControl[1+i],LOW);
       digitalWrite(ledControl[2+i],LOW);
       break;
   }
  }
}

boolean flipLEDs(boolean flip, boolean input) {
  if (flip) {
    ledInput = !ledInput;
  } else {
    ledInput = input;
  }
  digitalWrite(ledControl[6], ledInput);
  
  return ledInput;
}