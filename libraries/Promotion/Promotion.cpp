#include "Promotion.h"

Promotion::Promotion(short pinUp, short pinDown, short pinSelect, Lcd *l) :
  Menu(pinUp, pinDown, pinSelect, 2, l)
{
  symbols[0] = 5;
  symbols[1] = 2;
}

void Promotion::reset() {
  lcd->blink();
  lcd->clear();
  lcd->setCursor(1,0);
  lcd->print("Promote to ");
  lcd->write(short(symbols[0]));
  lcd->setCursor(1,1);
  lcd->print("Promote to ");
  lcd->write(short(symbols[1]));
  Menu::reset();
}

short Promotion::loop() {
  short menuLoop = Menu::loop();
  if (menuLoop != 0) {
    lcd->noBlink();
  }
  return menuLoop;
}

void Promotion::writeLCD() {
  selection = symbols[item];
  lcd->setCursor(0, item);
}