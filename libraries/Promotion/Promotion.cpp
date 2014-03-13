#include "Promotion.h"

Promotion::Promotion(short pinUp, short pinDown, short pinSelect, LiquidCrystal *l) :
  Menu(pinUp, pinDown, pinSelect, 2, l)
{
  symbols[0] = 5;
  symbols[1] = 2;
}

void Promotion::writeLCD() {
  selection = byte(symbols[item]);
  Lcd::clearLine(lcd);
  lcd->print("Promote to ");
  lcd->write(selection);
}