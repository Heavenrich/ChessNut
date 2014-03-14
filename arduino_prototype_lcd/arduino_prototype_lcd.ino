#include <LiquidCrystal.h>
#include "Chess.h"
#include "Promotion.h"
#include "Symbols.h"
#include "Menu.h"
#include "Lcd.h"
#include "Leds.h"
#include "ClockMenu.h"
#include "ClockTimer.h"
#include "Clock.h"

char cols[8] = {
  'A','B','C','D','E','F','G','H'
};

short gridInput[8] = {
  35,34,33,32,31,30,29,28
};
short gridOutput[8] = {
  36,37,38,39,40,41,42,43
};
short ledControl[7] = {
  44,45,46,47,48,49,7
}; //A-C for row, A-C for column, input voltage

short nRows = 8;
short delayRead = 1;
short endTurn = 8;
short up = 9;
short down = 10;
short newGame = 11;
short loadGame = 12;
short redLed = 13;
boolean enableNew = true;
boolean enableLoad = true;
boolean enableEnd = true;
boolean enableSelect;
short gameType;
short whiteTime;
short blackTime;

Lcd lcd(27, 26, 25, 24, 23, 22);
Leds leds(ledControl);
Chess chess(delayRead, endTurn, redLed, &lcd, &leds, cols, gridInput, gridOutput);
Promotion promotion(up, down, endTurn, &lcd);
ClockMenu clockMenu(up, down, endTurn, &lcd);
ClockTimer clockTimer(up, down, endTurn, &lcd);

// for controlling loop()
#define IDLE 0
#define NEW_GAME 1
#define LOAD_GAME 2
#define SCANNING 3
#define PROMOTION 4
#define CLOCK_MENU 5
#define CLOCK_TIMER_WHITE 6
#define CLOCK_TIMER_BLACK 7
#define CLOCK_START 8
#define END_GAME 9
short state;

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
  pinMode(redLed, OUTPUT);
  digitalWrite(redLed, LOW);
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
    chess.setRed(false);
    enableNew = false;
    state = CLOCK_MENU;
    clockMenu.reset();
  } else if (!digitalRead(newGame)) {
    enableNew = true;
  }

  if (enableLoad && digitalRead(loadGame)) {
    chess.setRed(false);
    enableLoad = false;
    //state = LOAD_GAME;
  } else if (!digitalRead(newGame)) {
    enableLoad = true;
  }

  if (state == NEW_GAME) {
    if (chess.initialize()) {
      chess.startGame();
      if (gameType == ClockMenu::timer) {
        state = CLOCK_START;
        enableSelect = false;
      } else {
        state = SCANNING;
      }
    }
  } else if (state == SCANNING) {
    short chessLoop = chess.loop();
    if (chessLoop == Chess::loop_promotion) {
      state = PROMOTION;
      promotion.reset();
    } else if (chessLoop == Chess::loop_timeout) {
      state = IDLE;
    }
  } else if (state == PROMOTION) {
    short promotedPiece = promotion.loop();
    if (promotedPiece != 0) {
      state = SCANNING;
      chess.setPromotedPiece(promotedPiece);
    }
  } else if (state == CLOCK_MENU) {
    gameType = clockMenu.loop();
    if (gameType == ClockMenu::timer) {
      state = CLOCK_TIMER_WHITE;
      clockTimer.reset();
    } else if (gameType == ClockMenu::noTimer) {
      state = NEW_GAME;
      if (chess.newGame()) {
        state = SCANNING;
      }
    }
  } else if (state == CLOCK_TIMER_WHITE) {
    whiteTime = clockTimer.loop();
    if (whiteTime != 0) {
      state = CLOCK_TIMER_BLACK;
      clockTimer.reset("black", whiteTime);
    }
  } else if (state == CLOCK_TIMER_BLACK) {
    blackTime = clockTimer.loop();
    if (blackTime != 0) {
      if (chess.newGame(whiteTime, blackTime)) {
        state = SCANNING;
      } else {
        state = NEW_GAME;
      }
    }
  } else if (state == CLOCK_START) {
    if (enableSelect && digitalRead(endTurn)) {
      if (chess.startClock()) {
        state = SCANNING;
      } else {
        state = NEW_GAME;
      }
    } else if (!digitalRead(endTurn)) {
      enableSelect = true;
    }
  }
}
