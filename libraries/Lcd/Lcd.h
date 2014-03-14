#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>
#include <LiquidCrystal.h>

class Lcd : public LiquidCrystal {
  public:
    Lcd(short pin1, short pin2, short pin3, short pin4, short pin5, short pin6);
    void clearLine(short row = 0);
    void writeClock(short whosTurn, short seconds);
};

#endif