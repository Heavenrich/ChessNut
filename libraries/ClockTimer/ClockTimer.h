#ifndef CLOCK_TIMER_H
#define CLOCK_TIMER_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "Menu.h"
#include "Lcd.h"

class ClockTimer : public Menu {
  public:
    ClockTimer(short pinUp, short pinDown, short pinSelect, LiquidCrystal *lcd);
    void reset(String player = "white", short time = 5);

  private:
    String times[26];
    String player;
    short time;
    
    void writeLCD();
};

#endif
