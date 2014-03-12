#include "Menu.h"

Menu::Menu(short pinUp, short pinDown, short pinSelect, short n, LiquidCrystal *l) :
  selection(0),
  item(0),
  nItems(n),
  up(pinUp),
  down(pinDown),
  select(pinSelect),
  enableUp(false),
  enableDown(false),
  enableSelect(false),
  lcd(l)
{
}

void Menu::reset() {
  item = 0;
  writeLCD();
}

short Menu::loop() {  
  if (digitalRead(select)){
    return selection;
  }
  
  if (enableUp && digitalRead(up)){
    item = (item + 1) % nItems;
    enableUp = false;
    writeLCD();
  } else if (!digitalRead(up)) {
    enableUp = true;
  }
  else if (enableDown && digitalRead(down)){
    item = (item + nItems - 1) % nItems;
    enableDown = false;
    writeLCD();
  } else if (!digitalRead(down)) {
    enableDown = true;
  }
  
  return 0;
}
