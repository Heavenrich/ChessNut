#include "ClockTimer.h"

ClockTimer::ClockTimer(short pinUp, short pinDown, short pinSelect, LiquidCrystal *l) :
  Menu(pinUp, pinDown, pinSelect, N_CLOCK_TIMES, l)
{
  String suffix = ":00";
  for (short j = 0; j < N_CLOCK_TIMES; j++) {
    if (j < 9) {
      times[j] = "0";
    } else {
      times[j] = "";
    }
    times[j] = times[j] + String(j + 1) + suffix;
  }
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
  lcd->print(times[item]);
}