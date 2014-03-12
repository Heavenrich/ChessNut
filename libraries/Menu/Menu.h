#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <LiquidCrystal.h>

class Menu {
  public:
    Menu(short pinUp, short pinDown, short pinSelect, short nItems, LiquidCrystal *lcd);
    void reset();
    short loop();
  protected:
    short selection;
    short item;
    const short nItems;
    const short up;
    const short down;
    const short select;
    boolean enableUp;
    boolean enableDown;
    boolean enableSelect;
    LiquidCrystal *lcd;
    
    virtual void writeLCD() = 0;
};

#endif
