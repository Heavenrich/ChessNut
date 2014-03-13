#ifndef CLOCK_TIMER_H
#define CLOCK_TIMER_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "Menu.h"
#include "Lcd.h"

#define N_CLOCK_TIMES 45

class ClockTimer : public Menu {
  public:
    ClockTimer(short pinUp, short pinDown, short pinSelect, LiquidCrystal *lcd);
    void reset(String player = "white", short time = 3);

  private:
    String times[45];
    String player;
    short time;
    
    void writeLCD();
};

#endif
