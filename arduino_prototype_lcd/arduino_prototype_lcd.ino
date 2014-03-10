#include <LiquidCrystal.h>
#include "Chess.h"
#include "Promotion.h"
#include "Symbols.h"

LiquidCrystal lcd(27, 26, 25, 24, 23, 22);

char cols[8] = {
  'A','B','C','D','E','F','G','H'
};

short gridInput[8] = {
  35,34,33,32,31,30,29,28
};
short gridOutput[8] = {
  36,37,38,39,40,41,42,43
};

Promotion promotion(1, 2, 3, &lcd); // TODO: need to set 1, 2, 3 to pins for buttons up, down, select

short nRows = 8;
short period = 50;
short endTurn = 11;
short scan = 12;

int ledControl[7] = {
  44,45,46,47,48,49,50
}; //A-C for row, A-C for column, input voltage

// for controlling loop()
#define IDLE 0
#define NEW_GAME 1
#define SCANNING 2
#define LOADING 3
#define PROMOTION 4
short state;

Chess chess(period, endTurn, scan, &lcd, cols, gridInput, gridOutput);

void setup() {
  Serial.begin(9600);
  
  for (short i = 0; i < nRows; i++) {
    pinMode(gridInput[i], INPUT);
    pinMode(gridOutput[i], OUTPUT);
  }
  digitalWrite(gridOutput[0], HIGH);
  pinMode(endTurn, INPUT);
  pinMode(scan, INPUT);
  Symbols::createChars(&lcd);
  lcd.begin(16, 2);
  state = NEW_GAME;
  
  //turn off LEDs if applicable
  for(short i = 0; i < 7; i++) {
    pinMode(ledControl[i], OUTPUT);
    digitalWrite(ledControl[i], LOW);
  }
}

void loop() {
  if (state == NEW_GAME) {
    if (chess.initialize()) {
      state = SCANNING;
      chess.newGame();
    }
  } else if (state == SCANNING) {
    chess.loop();
  } else if (state == PROMOTION) {
    short promotedPiece = promotion.loop();
    if (promotedPiece != 0) {
      state = SCANNING;
    }
  }
}
