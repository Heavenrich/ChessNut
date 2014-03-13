#include "ClockMenu.h"

ClockMenu::ClockMenu(short pinUp, short pinDown, short pinSelect, Lcd *l) :
  Menu(pinUp, pinDown, pinSelect, 2, l)
{
}

void ClockMenu::reset() {
  lcd->blink();
  lcd->clear();
  lcd->setCursor(1,0);
  lcd->print("Don't use clock");
  lcd->setCursor(1,1);
  lcd->print("Use a clock");
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