#ifndef PROMOTION_H
#define PROMOTION_H

#include <Arduino.h>
#include <LiquidCrystal.h>

class Promotion {
  public:
    Promotion(int pinUp, int pinDown, int pinSelect, LiquidCrystal *lcd);
    void reset();
    char loop();
  private:
    String statements[2];
    char symbols[2];
    short i;
    const int n;
    char up;
    char down;
    char select;
    boolean enableUp;
    boolean enableDown;
    boolean enableSelect;
    LiquidCrystal *lcd;
    
    void writeLCD();
};

#endif
