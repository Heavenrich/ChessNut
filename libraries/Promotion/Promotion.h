#ifndef PROMOTION_H
#define PROMOTION_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "Menu.h"

class Promotion : public Menu {
  public:
    Promotion(int pinUp, int pinDown, int pinSelect, LiquidCrystal *lcd);
  private:
    String statements[2];
    short symbols[2];
    
    void writeLCD(short i);
};

#endif
