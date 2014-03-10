#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <Arduino.h>
#include <LiquidCrystal.h>

class Symbols {
  public:
    Symbols();
    static void createChars(LiquidCrystal *lcd);
};

#endif