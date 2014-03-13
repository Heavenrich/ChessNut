#include "Lcd.h"

Lcd::Lcd(LiquidCrystal *l) :
  lcd(l)
{
}

void Lcd::clearLine(LiquidCrystal *l, short row) {
  l->setCursor(0, row);
  l->print("                ");
  l->setCursor(0, row);
}

void Lcd::clearLine(short row) {
  lcd->setCursor(0, row);
  lcd->print("                ");
  lcd->setCursor(0, row);
}

void Lcd::writeClock(LiquidCrystal *l, short whosTurn, short seconds) {
  String toLCD = "";
  if (short(seconds / 60) < 10) {
    toLCD = "0";
  }
  toLCD = toLCD + String(short(seconds / 60)) + ":";
  if (seconds % 60 < 10) {
    toLCD = toLCD + "0";
  }
  toLCD = toLCD + String(seconds % 60);

  if (whosTurn == 1) {
    l->setCursor(1, 1);
  } else {
    l->setCursor(10, 1);
  }
  l->print(toLCD);
}

void Lcd::writeClock(short whosTurn, short seconds) {
  String toLCD = "";
  if (short(seconds / 60) < 10) {
    toLCD = "0";
  }
  toLCD = toLCD + String(short(seconds / 60)) + ":";
  if (seconds % 60 < 10) {
    toLCD = toLCD + "0";
  }
  toLCD = toLCD + String(seconds % 60);

  if (whosTurn == 1) {
    lcd->setCursor(1, 1);
  } else {
    lcd->setCursor(10, 1);
  }
  lcd->print(toLCD);
}