#ifndef Promotion_h
#define Promotion_h

#include "Arduino.h"

class Promotion {
  public:
    Promotion(String statement, int symbol);
    String statement;
    int symbol;
};

#endif
