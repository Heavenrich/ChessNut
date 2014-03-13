#include "ClockTimer.h"

ClockTimer::ClockTimer(short pinUp, short pinDown, short pinSelect, LiquidCrystal *l) :
  Menu(pinUp, pinDown, pinSelect, 45, l)
{
}

void ClockTimer::reset(String p, short t) {
  player = p;
  time = t;
  Menu::reset(time - 1);
}

void ClockTimer::writeLCD() {
  selection = item + 1;
  lcd->clear();
  lcd->print("Time for " + player + ":");
  lcd->setCursor(1, 1);
  String prefix = "";
  if (selection < 10) {
    prefix = "0";
  }
  lcd->print(prefix + String(selection) + ":00");
}