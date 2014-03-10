#include <LiquidCrystal.h>
#include "Promotion.h"
#include "Symbols.h"

LiquidCrystal lcd(27, 26, 25, 24, 23, 22);

char pawn = 1;
char knight = 2;
char bishop = 3;
char rook = 4;
char queen = 5;
char king = 6;

char prevScan[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};

char currScan[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};

char diff[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};

char board[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};  

char cols[8] = {
  'A','B','C','D','E','F','G','H'
};

char columnShift[8] = {
  35,34,33,32,31,30,29,28
};

Promotion promotion(1, 2, 3, &lcd); // TODO: need to set 1, 2, 3 to pins for buttons up, down, select

char period = 50;
char nRows = 8;
char endTurn = 11;
boolean enableEndTurn = true;
char scan = 12;
boolean enableScan = true;

int ledControl[7] = {
  44,45,46,47,48,49,50
}; //A-C for row, A-C for column, input voltage

// true if a game is underway
boolean gameOn = false;

// if 1, then white's turn, if -1 then black's turn
char whosTurn = 1;

// list of moves made this turn 
// moves[0] = {-1, 2, 4} means the first move of the turn was a piece being removed from D2
// moves[1] = {1, 4, 4} means the second move of the turn was a piece being placed at D4
#define MAX_MOVES 20
char moves[MAX_MOVES][3];
char numMoves = 0;

// for controlling loop()
#define IDLE 0
#define NEW_GAME 1
#define SCANNING 2
#define LOADING 3
#define PROMOTION 4
char state;

void setup() {
  Serial.begin(9600);
  
  for (char i = 0; i < nRows; i++) {
    pinMode(columnShift[i], INPUT);
    pinMode(i+36, OUTPUT);
  }
  pinMode(endTurn, INPUT);
  pinMode(scan, INPUT);
  Symbols symbols;
  symbols.createChars(&lcd);
  lcd.begin(16, 2);
  state = NEW_GAME;
  
  //turn off LEDs if applicable
  for(int i = 0; i < 7; i++) {
    digitalWrite(ledControl[i],LOW);
  }
}

void loop() {
  if (state == NEW_GAME) {
    if (initialize()) {
      lcd.clear();
      lcd.print("ready to start!");
      gameOn = true;
      state = SCANNING;
    }
  } else if (state == SCANNING) {
    if (enableEndTurn && digitalRead(endTurn)) {
      if (turnEnd()) {
        whosTurn *= -1;
        numMoves = 0;
      }
      enableEndTurn = false;
    } else if (!digitalRead(endTurn)) {
      enableEndTurn = true;
    }
    if (scanBoard(false, false)) {
      setDiff();
      memcpy(prevScan, currScan, sizeof(prevScan));
      detectMove();
    }
  } else if (state == PROMOTION) {
    promotion.loop();
  }
}

// determine if the board has been set up in the standard starting position
// if it has, then initialize the board matrix and return true
// else return false
boolean initialize() {
  boolean initialized = true;
  for (char i=0; i < nRows; i ++) {
    for (char j=0; j < nRows; j++) {
      if (i == 0 || i == 1 || i == 6 || i == 7) {
        if (currScan[i][j] != 1) {
          initialized = false;
          break;
        } 
      } else if (currScan[i][j] == 1) {
          initialized = false;
          break;
      }
    }
  }
  
  if (initialized) {
    memcpy(board, (char[8][8]){
      {rook, knight, bishop, queen, king, bishop, knight, rook},
      {pawn, pawn, pawn, pawn, pawn, pawn, pawn, pawn},
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0},
      {-1*pawn, -1*pawn, -1*pawn, -1*pawn, -1*pawn, -1*pawn, -1*pawn, -1*pawn},
      {-1*rook, -1*knight, -1*bishop, -1*queen, -1*king, -1*bishop, -1*knight, -1*rook}
    }, sizeof(board));
  } 
 
  return initialized;
}

boolean scanBoard(boolean output, boolean continuous) {
  boolean scanned = false;
  if ((enableScan && digitalRead(scan)) || continuous){
    enableScan = false;
    // memcpy(prevScan, currScan, sizeof(prevScan));
    for (char i = 0; i < nRows; i++) {
      scanRow(i);
    }
    if (output) {
      outputBoard();
    }
    
    scanned = true;
  } else if (!digitalRead(scan)) {
    enableScan = true;
  }
  
  return scanned;
}

void scanRow(int row) {
  digitalWrite(row + 36, HIGH);
  for (char i = 0; i < nRows; i++) {
    if (i != row) {
      digitalWrite(i+36, LOW);
    }
  }
  
  delay(period);
  
  for (char i = 0; i < nRows; i++) {
    currScan[row][i] = digitalRead(columnShift[i]);
  }
}

// fill the diff matrix with current - previous scans
void setDiff() {
  for (char i = 0; i < nRows; i++) {
    for (char j = 0; j < nRows; j++) {
      diff[i][j] = currScan[i][j] - prevScan[i][j];
    }
  }
}

// analyze diff and save new moves made
void detectMove() {
  boolean made;
  char state;
  char relativeDiff;
  for (char i=0; i < nRows; i++) {
    for (char j=0; j < nRows; j++) {
      if (diff[i][j] != 0) {
        if (numMoves < MAX_MOVES) {
          moves[numMoves][0] = diff[i][j];
          moves[numMoves][1] = i;
          moves[numMoves][2] = j;
          numMoves++;
        }
        else {
          // TOO MANY MOVES????
        }
      }
    }
  }
}

boolean turnEnd() {
  Serial.print("turn: ");
  Serial.println(whosTurn);
  Serial.print("numMoves: ");
  Serial.println(numMoves);
  for (char i=0; i < numMoves; i++) {
    Serial.print("Move ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(moves[i][0]);
    Serial.print(cols[moves[i][2]]);
    Serial.println(moves[i][1]+1);
  }
  reduceMoves();
  Serial.print("numMoves reduced: ");
  Serial.println(numMoves);
  for (char i=0; i < numMoves; i++) {
    Serial.print("Move ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(moves[i][0]);
    Serial.print(cols[moves[i][2]]);
    Serial.println(moves[i][1]+1);
  }
  
  if (numMoves == 2) {
    if (
      moves[0][0] != -1 
      || moves[1][0] != 1
      || board[moves[0][1]][moves[0][2]]*whosTurn < 0
    ) {
      lcd.clear();
      lcd.print("invalid move");
      return false;
    }
    
    lcd.clear();
    if (whosTurn < 0) {
      lcd.print("B");
    } else {
      lcd.print("W");
    }
    debugScan(prevScan);
    lcd.write(byte(whosTurn*board[moves[0][1]][moves[0][2]]));
    lcd.print(" ");
    lcd.print(cols[moves[0][2]]);
    lcd.print(moves[0][1]+1);
    lcd.print(" to ");
    lcd.print(cols[moves[1][2]]);
    lcd.print(moves[1][1]+1);
    board[moves[1][1]][moves[1][2]] = board[moves[0][1]][moves[0][2]];
    board[moves[0][1]][moves[0][2]] = 0;   
    memcpy(prevScan, currScan, sizeof(prevScan));
    debugScan(prevScan);
    
    return true;
  }
  
  lcd.clear();
  lcd.print("too many moves");
  return false;
}

void reduceMoves() {
  char numReduced = 0;
  char reduced[MAX_MOVES][3];
  char diffSum = 0;
  for (char i = 0; i < numMoves; i++) {
    diffSum += moves[i][0];
  }
  if (diffSum == 0) {
    for (char i = 0; i < numMoves; i++) {
      boolean match = false;
      for (char j = 0; j < numReduced && !match; j++) {
        if (moves[i][1] == reduced[j][1]
            && moves[i][2] == reduced[j][2]
        ) {
          match = true;
          reduced[j][0] += moves[i][0];
        }
      }
      if (!match && numReduced < MAX_MOVES) {
        memcpy(reduced[numReduced], moves[i], sizeof(moves[i]));
        numReduced++;
      }
    }
  }
  
  numMoves = numReduced;
  memcpy(moves, reduced, sizeof(char)*3*numReduced);
}

void debugBoard(char array[8][8]) {
  Serial.println("------------------------");
  Serial.println("    A B C D E F G H     ");
  for (char i = nRows - 1; i >= 0; i--) {
    Serial.print(String(i + 1) + "   ");
    for (int j = 0; j < nRows; j++) {
      if (array[i][j] > 0) {
        Serial.print("W");
      } else {
        Serial.print("B");
      }
      Serial.print(array[i][j]);
    }
    Serial.println("   " + String(i + 1));
  }
  Serial.println("    A B C D E F G H     ");
}

void debugScan(char array[8][8]) {
  Serial.println("------------");
  Serial.println("  ABCDEFGH  ");
  for (char i = nRows - 1; i >= 0; i--) {
    Serial.print(String(i + 1) + " ");
    for (int j = 0; j < nRows; j++) {
      Serial.print(array[i][j]);
    }
    Serial.println(" " + String(i + 1));
  }
  Serial.println("  ABCDEFGH  ");
}

void outputBoard() {
  debugScan(currScan);
  
  lcd.clear();
  lcd.setCursor(1,0);
  char count = 0;
  for (char i = 0; i < nRows; i++) {
    for (char j = 0; j < nRows; j++) {
      if (currScan[i][j] == HIGH) {
        lcd.print(cols[j]);
        lcd.print(i+1);
        lcd.print(" ");
        count++;
      }
      if (count == 5) {
        lcd.setCursor(1,1);
      }
    }
  }
  if (count == 0) {
    lcd.write("nothing...");
  }
}

void setLEDPattern(int row, int column) {
  int tmp[2] = {row, column};
  for(int i = 0; i < 4; i+=3) {
    switch(tmp[i]) {
       case 1:
         digitalWrite(ledControl[0+i],LOW);
         digitalWrite(ledControl[1+i],LOW);
         digitalWrite(ledControl[2+i],LOW);
         break;
      case 2:
         digitalWrite(ledControl[0+i],LOW);
         digitalWrite(ledControl[1+i],LOW);
         digitalWrite(ledControl[2+i],HIGH);
         break;
      case 3:
         digitalWrite(ledControl[0+i],LOW);
         digitalWrite(ledControl[1+i],HIGH);
         digitalWrite(ledControl[2+i],LOW);
         break;
      case 4:
         digitalWrite(ledControl[0+i],LOW);
         digitalWrite(ledControl[1+i],HIGH);
         digitalWrite(ledControl[2+i],HIGH);
         break;
      case 5:
         digitalWrite(ledControl[0+i],HIGH);
         digitalWrite(ledControl[1+i],LOW);
         digitalWrite(ledControl[2+i],LOW);
         break;
      case 6:
         digitalWrite(ledControl[0+i],HIGH);
         digitalWrite(ledControl[1+i],LOW);
         digitalWrite(ledControl[2+i],HIGH);
         break;
      case 7:
         digitalWrite(ledControl[0+i],HIGH);
         digitalWrite(ledControl[1+i],HIGH);
         digitalWrite(ledControl[2+i],LOW);
         break;
      case 8:
         digitalWrite(ledControl[0+i],HIGH);
         digitalWrite(ledControl[1+i],HIGH);
         digitalWrite(ledControl[2+i],HIGH);
         break;
       default:
         digitalWrite(ledControl[0+i],LOW);
         digitalWrite(ledControl[1+i],LOW);
         digitalWrite(ledControl[2+i],LOW);
         break;
     }
  }
}

void flipLEDs() {
  digitalWrite(ledControl[6],!(digitalRead(ledControl[6])));
}
