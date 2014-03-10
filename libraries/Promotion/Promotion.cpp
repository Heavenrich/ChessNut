#include "Promotion.h"

Promotion::Promotion(int pinUp, int pinDown, int pinSelect, LiquidCrystal *l) :
  i(0),
  n(2),
  up(pinUp),
  down(pinDown),
  select(pinSelect),
  enableUp(true),
  enableDown(true),
  enableSelect(true),
  lcd(l)
{
  statements[0] = "Promote to Q?";
  statements[1] = "Promote to N?";

  symbols[0] = 5;
  symbols[1] = 1;
}

void Promotion::reset() {
  i = 0;
  writeLCD();
}

void Promotion::writeLCD() {
  lcd->clear();
  lcd->setCursor(0,0);
  lcd->print(statements[i] + " ");
  lcd->write(byte(symbols[i]));
}

short Promotion::loop() {  
  if (digitalRead(select)){
    return symbols[i];
  }
  
  if (enableUp && digitalRead(up)){
    i = (i + 1) % n;
    enableUp = false;
    writeLCD();
  } else if (!digitalRead(up)) {
    enableUp = true;
  }
  
  if (enableDown && digitalRead(down)){
    i = (i + n - 1) % n;
    enableDown = false;
    writeLCD();
  } else if (!digitalRead(down)) {
    enableDown = true;
  }
  
  return 0;
}
