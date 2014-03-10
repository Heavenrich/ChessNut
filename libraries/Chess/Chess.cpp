#include "Chess.h"

Chess::Chess() :
  pawn(1),
  knight(2),
  bishop(3),
  rook(4),
  queen(5),
  king(6),
  period(50),
  nRows(8),
  endTurn(11),
  scan(12)
{}

Chess::Chess(short p, short end, short s, LiquidCrystal *l, char colLetters[8], short inPins[8], short outPins[8]) :
  pawn(1),
  knight(2),
  bishop(3),
  rook(4),
  queen(5),
  king(6),
  period(p),
  nRows(8),
  endTurn(end),
  enableEndTurn(true),
  scan(s),
  enableScan(true),
  gameOn(false),
  whosTurn(1),
  numMoves(0),
  lcd(l)
{
  for (short i = 0; i < nRows; i++) {
    cols[i] = colLetters[i];
    gridInput[i] = inPins[i];
    gridOutput[i] = outPins[i];
    currScan[0][i] = 0;
  }
}

// determine if the board has been set up in the standard starting position
// if it has, then initialize the board matrix and return true
// else return false
boolean Chess::initialize() {
  scanBoard(false, false);
  boolean initialized = true;
  for (short i=0; i < nRows && initialized; i ++) {
    for (short j=0; j < nRows; j++) {
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
    memcpy(prevScan, currScan, sizeof(prevScan));
    memcpy(board, (short[8][8]){
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
 
  Serial.println("initialized" + String(initialized));
  return initialized;
}

void Chess::newGame() {
  lcd->clear();
  lcd->print("ready to start!");
  Serial.println("ready to start!");
  gameOn = true;
}

boolean Chess::loop() {
  if (enableEndTurn && digitalRead(endTurn)) {
    if (turnEnd()) {
      whosTurn *= -1;
      numMoves = 0;
      return true;
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
  
  return false;
}

boolean Chess::scanBoard(boolean continuous, boolean output) {
  boolean scanned = false;
  if ((enableScan && digitalRead(scan)) || continuous){
    enableScan = false;
    for (short i = 0; i < nRows; i++) {
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

void Chess::scanRow(short row) {
  digitalWrite(gridOutput[row], HIGH);
  for (short i = 0; i < nRows; i++) {
    if (i != row) {
      digitalWrite(gridOutput[i], LOW);
    }
  }
  
  delay(period);
  
  for (short i = 0; i < nRows; i++) {
    currScan[row][i] = digitalRead(gridInput[i]);
  }
}

// fill the diff matrix with current - previous scans
void Chess::setDiff() {
  for (short i = 0; i < nRows; i++) {
    for (short j = 0; j < nRows; j++) {
      diff[i][j] = currScan[i][j] - prevScan[i][j];
    }
  }
}

// analyze diff and save new moves made
void Chess::detectMove() {
  for (short i=0; i < nRows; i++) {
    for (short j=0; j < nRows; j++) {
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

boolean Chess::turnEnd() {
  Serial.print("turn: ");
  Serial.println(whosTurn);
  Serial.print("numMoves: ");
  Serial.println(numMoves);
  for (short i=0; i < numMoves; i++) {
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
  for (short i=0; i < numMoves; i++) {
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
      lcd->clear();
      lcd->print("invalid move");
      return false;
    }
    
    lcd->clear();
    if (whosTurn < 0) {
      lcd->print("B");
    } else {
      lcd->print("W");
    }
    debugScan(prevScan);
    lcd->write(byte(whosTurn*board[moves[0][1]][moves[0][2]]));
    lcd->print(" ");
    lcd->print(cols[moves[0][2]]);
    lcd->print(moves[0][1]+1);
    lcd->print(" to ");
    lcd->print(cols[moves[1][2]]);
    lcd->print(moves[1][1]+1);
    board[moves[1][1]][moves[1][2]] = board[moves[0][1]][moves[0][2]];
    board[moves[0][1]][moves[0][2]] = 0;
    
    return true;
  }
  
  lcd->clear();
  lcd->print("too many moves");
  return false;
}

void Chess::reduceMoves() {
  short numReduced = 0;
  short reduced[MAX_MOVES][3];
  short diffSum = 0;
  for (short i = 0; i < numMoves; i++) {
    diffSum += moves[i][0];
  }
  if (diffSum == 0) {
    for (short i = 0; i < numMoves; i++) {
      boolean match = false;
      for (short j = 0; j < numReduced && !match; j++) {
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
  
  numMoves = 0;
  for (short i = 0; i < numReduced; i++) {
    if (reduced[i][0] != 0) {
      memcpy(moves[numMoves], reduced[i], sizeof(reduced[i]));
      numMoves++;
    }
  }
}

void Chess::debugBoard(short array[8][8]) {
  Serial.println("------------------------");
  Serial.println("    A B C D E F G H     ");
  for (short i = nRows - 1; i >= 0; i--) {
    Serial.print(String(i + 1) + "   ");
    for (int j = 0; j < nRows; j++) {
      if (array[i][j] > 0) {
        Serial.print("W");
      } else if (array[i][j] < 0) {
        Serial.print("B");
      } else {
        Serial.print(" ");
      }
      Serial.print(array[i][j]);
    }
    Serial.println("   " + String(i + 1));
  }
  Serial.println("    A B C D E F G H     ");
}

void Chess::debugScan(short array[8][8]) {
  Serial.println("------------");
  Serial.println("  ABCDEFGH  ");
  for (short i = nRows - 1; i >= 0; i--) {
    Serial.print(String(i + 1) + " ");
    for (int j = 0; j < nRows; j++) {
      Serial.print(array[i][j]);
    }
    Serial.println(" " + String(i + 1));
  }
  Serial.println("  ABCDEFGH  ");
}

void Chess::outputBoard() {
  debugScan(currScan);
  
  lcd->clear();
  lcd->setCursor(1,0);
  short count = 0;
  for (short i = 0; i < nRows; i++) {
    for (short j = 0; j < nRows; j++) {
      if (currScan[i][j] == HIGH) {
        lcd->print(cols[j]);
        lcd->print(i+1);
        lcd->print(" ");
        count++;
      }
      if (count == 5) {
        lcd->setCursor(1,1);
      }
    }
  }
  if (count == 0) {
    lcd->write("nothing...");
  }
}