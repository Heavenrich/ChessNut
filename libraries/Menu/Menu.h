#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include "Lcd.h"

class Menu {
  public:
    Menu(short pinUp, short pinDown, short pinSelect, short nItems, Lcd *lcd);
    virtual void reset(short item = 0);
    virtual short loop();
  protected:
    short selection; // variable that is to be returned
    short item; // index
    const short nItems;
    const short up;
    const short down;
    const short select;
    boolean enableUp;
    boolean enableDown;
    boolean enableSelect;
    Lcd *lcd;
    
    virtual void writeLCD() = 0;
};

#endif
