#include "Promotion.h"

Promotion::Promotion(short pinUp, short pinDown, short pinSelect, Lcd *l) :
  Menu(pinUp, pinDown, pinSelect, 2, l)
{
  symbols[0] = 5;
  symbols[1] = 2;
}

void Promotion::reset() {
  lcd->blink();
  lcd->clearLine();
  lcd->print("Promote to Q");
  lcd->write(short(symbols[0]));
  lcd->write(" N");
  lcd->write(short(symbols[1]));
  Menu::reset();
}

short Promotion::loop() {
  short menuLoop = Menu::loop();
  if (menuLoop != 0) {
    lcd->noBlink();
    lcd->clearLine();
    lcd->print("Promoted to ");
    lcd->write(selection);
  }
  return menuLoop;
}

void Promotion::writeLCD() {
  selection = symbols[item];
  lcd->setCursor(11 + item * 3, 0);
}