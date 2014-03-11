#include "Chess.h"

Chess::Chess(short d, short end, short s, LiquidCrystal *l, char colLetters[8], short inPins[8], short outPins[8]) :
  pawn(1),
  knight(2),
  bishop(3),
  rook(4),
  queen(5),
  king(6),
  delayRead(d),
  nRows(8),
  endTurn(end),
  enableEndTurn(true),
  scan(s),
  enableScan(true),
  gameOn(false),
  whosTurn(1),
  numMoves(0),
  castlingKing(0),
  castlingQueen(1),
  lcd(l)
{
  for (short i = 0; i < nRows; i++) {
    cols[i] = colLetters[i];
    gridInput[i] = inPins[i];
    gridOutput[i] = outPins[i];
    currScan[0][i] = 0;
  }
  castlingDepartures[castlingKing][0] = 4;
  castlingDepartures[castlingKing][1] = 7;
  castlingDepartures[castlingQueen][0] = 4;
  castlingDepartures[castlingQueen][1] = 0;
  castlingArrivals[castlingKing][0] = 6;
  castlingArrivals[castlingKing][1] = 5;
  castlingArrivals[castlingQueen][0] = 2;
  castlingArrivals[castlingQueen][1] = 3;
}

// determine if the board has been set up in the standard starting position
// if it has, then initialize the board matrix and return true
// else return false
boolean Chess::initialize() {
  boolean initialized = false;
  if (scanBoard(true, true)) {
    initialized = true;
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
 
  return initialized;
}

void Chess::newGame() {
  lcd->clear();
  lcd->print("ready to start!");
  Serial.println("ready to start!");
  gameOn = true;
}

short Chess::loop() {
  if (enableEndTurn && digitalRead(endTurn)) {
    enableEndTurn = false;
    if (turnEnd()) {
      numMoves = 0;
      if (moves[1][1] == (whosTurn + 1) * 7 / 2
          && abs(board[moves[1][1]][moves[1][2]]) == pawn
      ) {
        Serial.println("promotion");
        return loop_promotion;
      } else {
        Serial.println("endTurn");
        whosTurn *= -1;
        return loop_endTurn;
      }
    }
  } else if (!digitalRead(endTurn)) {
    enableEndTurn = true;
  }
  if (scanBoard(true, false)) {
    setDiff();
    memcpy(prevScan, currScan, sizeof(prevScan));
    detectMove();
  }
  
  return loop_noUpdate;
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

void Chess::setPromotedPiece(short piece) {
  Serial.println("pawn promoted to " + String(whosTurn * piece));
  board[moves[1][1]][moves[1][2]] = whosTurn * piece;
  whosTurn *= -1;
}

void Chess::scanRow(short row) {
  digitalWrite(gridOutput[row], HIGH);
  for (short i = 0; i < nRows; i++) {
    if (i != row) {
      digitalWrite(gridOutput[i], LOW);
    }
  }
  
  delay(delayRead);
  
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
          Serial.print("Move ");
          Serial.print(numMoves);
          Serial.print(": ");
          Serial.print(moves[numMoves][0]);
          Serial.print(cols[j]);
          Serial.print(i+1);
          Serial.print(" detected at millis() = ");
          Serial.println(millis());
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
  
  if (numMoves == 0) {
    lcd->clear();
    lcd->print("no moves");
    return false;
  } if (numMoves == 2) {
    short fromIndex = 0;
    if (moves[0][1] == -1) {
      fromIndex = 1;
    }
    if (
      moves[0][0] * moves[1][0] > 0
      || board[moves[fromIndex][1]][moves[fromIndex][2]]*whosTurn <= 0
      || board[moves[abs(fromIndex - 1)][1]][moves[abs(fromIndex - 1)][2]]*whosTurn > 0
    ) {
      lcd->clear();
      lcd->print("invalid move");
      Serial.println("invalid move " + String(board[moves[0][1]][moves[0][2]]) + " " + String(whosTurn));
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
  // castling
  else if (numMoves == 4) {
    boolean validCastle = true;
    boolean validKingSide = true;
    boolean validQueenSide = true;
    short castleColumn = (1 - whosTurn) * 7 / 2;
    for (short i = 0; i < numMoves; i++) {
      if (moves[i][1] != castleColumn) {
        validCastle = false;
        Serial.println("validCastle failed");
      }
      if (moves[i][0] < 0) {
        if (moves[i][2] != castlingDepartures[castlingKing][0]
            && moves[i][2] != castlingDepartures[castlingKing][1])
        {
          validKingSide = false;
        }
        if (moves[i][2] != castlingDepartures[castlingQueen][0]
            && moves[i][2] != castlingDepartures[castlingQueen][1])
        {
          validQueenSide = false;
        }
      } else {
        if (moves[i][2] != castlingArrivals[castlingKing][0]
            && moves[i][2] != castlingArrivals[castlingKing][1])
        {
          validKingSide = false;
        }
        if (moves[i][2] != castlingArrivals[castlingQueen][0]
            && moves[i][2] != castlingArrivals[castlingQueen][1])
        {
          validQueenSide = false;
        }
      }
    }
    if (validCastle && (validKingSide || validQueenSide)) {
      debugScan(prevScan);
      lcd->clear();
      if (whosTurn < 0) {
        lcd->print("B");
      } else {
        lcd->print("W");
      }
      lcd->print("O-O");
      if (validQueenSide) {
        lcd->print("-O");
        board[castleColumn][castlingArrivals[castlingQueen][0]] = king * whosTurn;
        board[castleColumn][castlingArrivals[castlingQueen][1]] = rook * whosTurn;
        board[castleColumn][castlingDepartures[castlingQueen][0]] = 0;
        board[castleColumn][castlingDepartures[castlingQueen][1]] = 0;
      } else {
        board[castleColumn][castlingArrivals[castlingKing][0]] = king * whosTurn;
        board[castleColumn][castlingArrivals[castlingKing][1]] = rook * whosTurn;
        board[castleColumn][castlingDepartures[castlingKing][0]] = 0;
        board[castleColumn][castlingDepartures[castlingKing][1]] = 0;
      }
      
      return true;
    }
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
  if (diffSum == 0 || diffSum == -1) {
    for (short i = numMoves - 1; i >= 0; i--) {
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
    if (diffSum == -1) {
      short oppositeTakes = 0;
      short oppositeTaken = 0;
      for (short i = 0; i < numReduced; i++) {
        // covers resonable cases (still edge cases) for correct taken piece
        if (reduced[i][0] == 0
            && board[reduced[i][1]][reduced[i][2]] * whosTurn < 0)
        {
          oppositeTakes++;
          oppositeTaken = i;
        }
      }
      if (oppositeTakes > 0) {
        reduced[oppositeTaken][0] = 1;
      }
    }

    numMoves = 0;
    for (short i = numReduced - 1; i >= 0; i--) {
      if (reduced[i][0] != 0) {
        memcpy(moves[numMoves], reduced[i], sizeof(reduced[i]));
        numMoves++;
      }
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