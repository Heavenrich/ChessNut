#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <Arduino.h>
#include "Lcd.h"

class Symbols {
  public:
    Symbols();
    static void createChars(Lcd *lcd);
};

#endif