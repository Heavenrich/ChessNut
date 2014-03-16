#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>

class Leds {
  public:
    Leds(short pins[8]);
    void setLEDPattern(short row, short column);
    void lightWhosTurn(short whosTurn);
    void errorWhosTurn(short whosTurn);
    void turnOff();
    
  private:
    boolean flipLEDs(boolean flip = true, boolean input = true);
    void setInput(boolean isColumn, boolean input);
    void setLED(boolean isColumn, short index);
    
    short *ledControl;
    boolean ledInput;
    
};

#endif