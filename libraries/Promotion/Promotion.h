#ifndef PROMOTION_H
#define PROMOTION_H

#include <Arduino.h>
#include "Menu.h"
#include "Lcd.h"

class Promotion : public Menu {
  public:
    Promotion(short pinUp, short pinDown, short pinSelect, Lcd *lcd);
    virtual void reset();
    virtual short loop();

  private:
    short symbols[2];
    
    void writeLCD();
};

#endif
