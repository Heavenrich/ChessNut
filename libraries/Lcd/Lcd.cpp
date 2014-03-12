#include "Lcd.h"

Lcd::Lcd()
{
}

void Lcd::clearLine(LiquidCrystal *lcd, short row) {
  lcd->setCursor(0, row);
  lcd->print("                ");
  lcd->setCursor(0, row);
}