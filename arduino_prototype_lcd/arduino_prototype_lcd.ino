#include <LiquidCrystal.h>
#include "Chess.h"
#include "Promotion.h"
#include "Symbols.h"
#include "Menu.h"

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

short nRows = 8;
short delayRead = 1;
short endTurn = 8;
short up = 9;
short down = 10;
short newGame = 11;
short loadGame = 12;
short endGame = 13;
short scan = 13;
boolean enableNew = true;
boolean enableLoad = true;
boolean enableEnd = true;

Promotion promotion(up, down, endTurn, &lcd);

int ledControl[7] = {
  44,45,46,47,48,49,50
}; //A-C for row, A-C for column, input voltage

// for controlling loop()
#define IDLE 0
#define NEW_GAME 1
#define LOAD_GAME 2
#define SCANNING 3
#define PROMOTION 4
#define END_GAME 5
short state;

Chess chess(delayRead, endTurn, scan, &lcd, cols, gridInput, gridOutput);

void setup() {
  Serial.begin(9600);
  
  for (short i = 0; i < nRows; i++) {
    pinMode(gridInput[i], INPUT);
    pinMode(gridOutput[i], OUTPUT);
  }
  pinMode(endTurn, INPUT);
  pinMode(up, INPUT);
  pinMode(down, INPUT);
  pinMode(newGame, INPUT);
  pinMode(loadGame, INPUT);
  pinMode(endGame, INPUT);
  Symbols::createChars(&lcd);
  lcd.begin(16, 2);
  state = IDLE;
  
  //turn off LEDs if applicable
  for(short i = 0; i < 7; i++) {
    pinMode(ledControl[i], OUTPUT);
    digitalWrite(ledControl[i], LOW);
  }
}

void loop() {
  if (enableNew && digitalRead(newGame)) {
    enableNew = false;
    state = NEW_GAME;
    if (chess.newGame()) {
      state = SCANNING;
    }
  } else if (!digitalRead(newGame)) {
    enableNew = true;
  }

  if (enableLoad && digitalRead(loadGame)) {
    enableLoad = false;
    //state = LOAD_GAME;
  } else if (!digitalRead(newGame)) {
    enableLoad = true;
  }

  if (state == NEW_GAME) {
    if (chess.initialize()) {
      state = SCANNING;
      chess.startGame();
    }
  } else if (state == SCANNING) {
    short chessLoop = chess.loop();
    if (chessLoop == Chess::loop_promotion) {
      state = PROMOTION;
      promotion.reset();
    }
  } else if (state == PROMOTION) {
    short promotedPiece = promotion.loop();
    if (promotedPiece != 0) {
      state = SCANNING;
      chess.setPromotedPiece(promotedPiece);
    }
  }
}
