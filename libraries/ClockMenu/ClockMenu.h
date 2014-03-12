#ifndef CLOCK_MENU_H
#define CLOCK_MENU_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "Menu.h"
#include "Lcd.h"

class ClockMenu : public Menu {
  public:
    ClockMenu(short pinUp, short pinDown, short pinSelect, LiquidCrystal *lcd);
    
    static const short timer = 2;
    static const short noTimer = 1;
  private:
    String statements[2];
    
    void writeLCD();
};

#endif
