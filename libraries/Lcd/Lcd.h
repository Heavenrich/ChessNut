#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>
#include <LiquidCrystal.h>

class Lcd {
  public:
    Lcd(LiquidCrystal *lcd);
    static void clearLine(LiquidCrystal *lcd, short row = 0);
    void clearLine(short row = 0);
    static void writeClock(LiquidCrystal *lcd, short whosTurn, short seconds);
    void writeClock(short whosTurn, short seconds);

  private:
    LiquidCrystal *lcd;
};

#endif