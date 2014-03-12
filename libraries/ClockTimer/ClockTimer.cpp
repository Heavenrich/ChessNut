#include "ClockTimer.h"

ClockTimer::ClockTimer(short pinUp, short pinDown, short pinSelect, LiquidCrystal *l) :
  Menu(pinUp, pinDown, pinSelect, 26, l)
{
  String suffix = ":00";
  for (short j = 0; j < 26; j++) {
    if (j < 5) {
      times[j] = "0";
    } else {
      times[j] = "";
    }
    times[j] = times[j] + String(j + 5) + suffix;
  }
}

void ClockTimer::reset(String p, short t) {
  player = p;
  time = t;
  Menu::reset(time - 5);
}

void ClockTimer::writeLCD() {
  selection = item + 5;
  lcd->clear();
  lcd->print("Time for " + player + ":");
  lcd->setCursor(1, 1);
  lcd->print(times[item]);
}