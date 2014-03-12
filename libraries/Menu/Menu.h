#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <LiquidCrystal.h>

class Menu {
  public:
    Menu(int pinUp, int pinDown, int pinSelect, LiquidCrystal *lcd);
    void reset();
    short loop();
  protected:
    short selection;
    short i;
    const int n;
    const short up;
    const short down;
    const short select;
    boolean enableUp;
    boolean enableDown;
    boolean enableSelect;
    LiquidCrystal *lcd;
    
    virtual void writeLCD(short i) = 0;
};

#endif
