#include "Menu.h"

Menu::Menu(int pinUp, int pinDown, int pinSelect, LiquidCrystal *l) :
  selection(0),
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
}

void Menu::reset() {
  i = 0;
  writeLCD(0);
}

short Menu::loop() {  
  if (digitalRead(select)){
    return selection;
  }
  
  if (enableUp && digitalRead(up)){
    i = (i + 1) % n;
    enableUp = false;
    writeLCD(i);
  } else if (!digitalRead(up)) {
    enableUp = true;
  }
  else if (enableDown && digitalRead(down)){
    i = (i + n - 1) % n;
    enableDown = false;
    writeLCD(i);
  } else if (!digitalRead(down)) {
    enableDown = true;
  }
  
  return 0;
}
