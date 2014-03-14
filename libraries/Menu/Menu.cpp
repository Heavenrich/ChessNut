#include "Menu.h"

Menu::Menu(short pinUp, short pinDown, short pinSelect, short n, Lcd *l) :
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

void Menu::reset(short i) {
  item = i;
  writeLCD();
  enableUp = false;
  enableDown = false;
  enableSelect = false;
}

short Menu::loop() {
  if (enableSelect && digitalRead(select)){
    return selection;
  } else if (!digitalRead(select)) {
    enableSelect = true;
  }
  
  if (enableUp && digitalRead(up)){
    item = (item + 1) % nItems;
    enableUp = false;
    writeLCD();
  } else if (!digitalRead(up)) {
    enableUp = true;
  }
  if (enableDown && digitalRead(down)){
    item = (item + nItems - 1) % nItems;
    enableDown = false;
    writeLCD();
  } else if (!digitalRead(down)) {
    enableDown = true;
  }
  
  return 0;
}
