#include "ClockMenu.h"

ClockMenu::ClockMenu(short pinUp, short pinDown, short pinSelect, LiquidCrystal *l) :
  Menu(pinUp, pinDown, pinSelect, 2, l)
{
  statements[noTimer - 1] = "Don't use clock";
  statements[timer - 1] = "Use a clock";
}

void ClockMenu::reset() {
  lcd->blink();
  lcd->clear();
  lcd->setCursor(1,0);
  lcd->print(statements[noTimer - 1]);
  lcd->setCursor(1,1);
  lcd->print(statements[timer - 1]);
  Menu::reset();
}

short ClockMenu::loop() {
  short menuLoop = Menu::loop();
  if (menuLoop != 0) {
    lcd->noBlink();
  }
  return menuLoop;
}

void ClockMenu::writeLCD() {
  selection = item + 1;
  lcd->setCursor(0, item);
}