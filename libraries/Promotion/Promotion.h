#ifndef PROMOTION_H
#define PROMOTION_H

#include <Arduino.h>
#include <LiquidCrystal.h>

class Promotion {
  public:
    Promotion(int pinUp, int pinDown, int pinSelect, LiquidCrystal *lcd);
    void reset();
    short loop();
  private:
    String statements[2];
    short symbols[2];
    short i;
    const int n;
    short up;
    short down;
    short select;
    boolean enableUp;
    boolean enableDown;
    boolean enableSelect;
    LiquidCrystal *lcd;
    
    void writeLCD();
};

#endif
