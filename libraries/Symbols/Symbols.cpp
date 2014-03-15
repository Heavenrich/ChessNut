#include "Symbols.h"

Symbols::Symbols()
{
}

void Symbols::createChars(Lcd *lcd) {
  byte pawn[8] = {
    B00000,
    B00000,
    B00000,
    B01100,
    B01100,
    B01100,
    B01100,
    B11110
  };

  byte knight[8] = {
    B00000,
    B01000,
    B01111,
    B01111,
    B01111,
    B01100,
    B01100,
    B11110
  };

  byte bishop[8] = {
    B00000,
    B00100,
    B01100,
    B01000,
    B01100,
    B01100,
    B01100,
    B11110
  };

  byte rook[8] = {
    B00000,
    B00000,
    B10101,
    B11111,
    B01110,
    B01110,
    B01110,
    B11111
  };

  byte queen[8] = {
    B01110,
    B01110,
    B01110,
    B01110,
    B01110,
    B01110,
    B01110,
    B11111
  };

  byte king[8] = {
    B00100,
    B01110,
    B00100,
    B01110,
    B01110,
    B01110,
    B01110,
    B11111
  };

  lcd->createChar(1, pawn);
  lcd->createChar(2, knight);
  lcd->createChar(3, bishop);
  lcd->createChar(4, rook);
  lcd->createChar(5, queen);
  lcd->createChar(6, king);
}