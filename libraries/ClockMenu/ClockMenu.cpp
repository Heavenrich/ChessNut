#include "ClockMenu.h"

ClockMenu::ClockMenu(short pinUp, short pinDown, short pinSelect, LiquidCrystal *l) :
  Menu(pinUp, pinDown, pinSelect, 2, l)
{
  statements[noTimer - 1] = "Don't use clock";
  statements[timer - 1] = "Use a clock";
}

void ClockMenu::writeLCD() {
  selection = item + 1;
  lcd->clear();
  lcd->print(statements[item]);
}