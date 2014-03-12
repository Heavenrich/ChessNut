#include "Promotion.h"

Promotion::Promotion(int pinUp, int pinDown, int pinSelect, LiquidCrystal *l) :
  Menu(pinUp, pinDown, pinSelect, l)
{
  statements[0] = "Promote to Q?";
  statements[1] = "Promote to N?";

  symbols[0] = 5;
  symbols[1] = 2;
}

void Promotion::writeLCD(short i) {
  selection = byte(symbols[i]);
  lcd->clear();
  lcd->print(statements[i] + " ");
  lcd->write(selection);
}