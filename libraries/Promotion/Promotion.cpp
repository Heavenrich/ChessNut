#include "Promotion.h"

Promotion::Promotion(short pinUp, short pinDown, short pinSelect, LiquidCrystal *l) :
  Menu(pinUp, pinDown, pinSelect, 2, l)
{
  statements[0] = "Promote to Q?";
  statements[1] = "Promote to N?";

  symbols[0] = 5;
  symbols[1] = 2;
}

void Promotion::writeLCD() {
  selection = byte(symbols[item]);
  Lcd::clearLine(lcd);
  lcd->print(statements[item] + " ");
  lcd->write(selection);
}