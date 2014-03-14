#include "Lcd.h"

Lcd::Lcd(short pin1, short pin2, short pin3, short pin4, short pin5, short pin6) :
  LiquidCrystal(pin1, pin2, pin3, pin4, pin5, pin6)
{
}

void Lcd::clearLine(short row) {
  setCursor(0, row);
  print("                ");
  setCursor(0, row);
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
    setCursor(1, 1);
  } else {
    setCursor(10, 1);
  }
  print(toLCD);
}