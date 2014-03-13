#ifndef CLOCK_H
#define CLOCK_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "Lcd.h"

class Clock {
  public:
    Clock(LiquidCrystal * lcd);
    void set(short whiteTime, short blackTime);
    void start();
    boolean loop(short whosTurn);
    
    boolean enabled;
  private:
    long whiteTime;
    long blackTime;
    long time;
    boolean started;
    Lcd lcd;
};

#endif