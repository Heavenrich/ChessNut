#ifndef CLOCK_MENU_H
#define CLOCK_MENU_H

#include <Arduino.h>
#include "Menu.h"
#include "Lcd.h"

class ClockMenu : public Menu {
  public:
    ClockMenu(short pinUp, short pinDown, short pinSelect, Lcd *lcd);
    virtual void reset();
    virtual short loop();
    
    static const short timer = 2;
    static const short noTimer = 1;

  private:
    void writeLCD();
};

#endif
