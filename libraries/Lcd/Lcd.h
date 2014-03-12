#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>
#include <LiquidCrystal.h>

class Lcd {
  public:
    Lcd();
    static void clearLine(LiquidCrystal *lcd, short row = 0);
};

#endif