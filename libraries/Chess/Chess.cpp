#include "Chess.h"
#include <stdio.h>

Chess::Chess(short d, short end, short red, Lcd *lc, Leds *led, char colLetters[8], short inPins[8], short outPins[8]) :
  leds(led),
  clock(lc),
  delayRead(d),
  nRows(8),
  endTurn(end),
  enableEndTurn(true),
  scan(d),
  enableScan(true),
  redLed(red),
  whosTurn(1),
  numMoves(0),
  castlingKing(0),
  castlingQueen(1),
  cols(colLetters),
  gridInput(inPins),
  gridOutput(outPins),
  lcd(lc)
{
  castlingDepartures[castlingKing][0] = 4;
  castlingDepartures[castlingKing][1] = 7;
  castlingDepartures[castlingQueen][0] = 4;
  castlingDepartures[castlingQueen][1] = 0;
  castlingArrivals[castlingKing][0] = 6;
  castlingArrivals[castlingKing][1] = 5;
  castlingArrivals[castlingQueen][0] = 2;
  castlingArrivals[castlingQueen][1] = 3;
  shortForms[pawn] = "P";
  shortForms[knight] = "N";
  shortForms[bishop] = "B";
  shortForms[rook] = "R";
  shortForms[queen] = "Q";
  shortForms[king] = "K";
}

// set board, is initialized? -> startGame()
boolean Chess::newGame(short whiteTime, short blackTime) {
  numMoves = 0;
  setWhosTurn(1);
  lcd->clear();
  clock.set(whiteTime, blackTime);
  resetFixes();
  initializeBoard();
  if (!initialize()) {
    return false;
  } else {
    startGame();
    return true;
  }
}

void Chess::initializeBoard() {
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

// determine if the board has been set up in the standard starting position
// if it has, then initialize the board matrix and return true
// else return false
boolean Chess::initialize() {
  boolean initialized = false;
  if (scanBoard(true, false)) {
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
    setRed(false);
    memcpy(prevScan, currScan, sizeof(prevScan));
    kingPositions[0][0] = 7;
    kingPositions[0][1] = 4;
    kingPositions[1][0] = 0;
    kingPositions[1][1] = 4;
    clock.writeTimes();
  } else {
    setRed(true);
    if (nFixes == 0) {
      Serial.println("set up to start!");
    }
    fixBoard("set up:");
  }

  return initialized;
}

// let game begin, or if clock let them press select to start the game
void Chess::startGame() {
  numTurns = 1;
  if (clock.enabled) {
    lcd->clearLine();
    lcd->print("press to start!");
  } else {
    lcd->clear();
    lcd->print("ready to start!");
  }
  Serial.println("ready to start!");
}

// if board is still initialized start the game
boolean Chess::startClock() {
  if (initialize()) {
    clock.start();
    lcd->clearLine();
    lcd->print("clock started!");
    return true;
  } else {
    fixBoard("set up:");
    Serial.println("set up to start!");
    return false;
  }
}

// main loop, return a loop_ variable if endTurn, promotion, or clock timeout
short Chess::loop() {
  if (!clock.loop(whosTurn)) {
    setRed(true);
    lcd->clear();
    lcd->print("Time ran out!");
    lcd->setCursor(0, 1);
    if (whosTurn == 1) {
      lcd->print("Black");
    } else {
      lcd->print("White");
    }
    lcd->print(" wins.");
    leds->turnOff();
    return loop_timeout;
  }
  if (enableEndTurn && digitalRead(endTurn)) {
    enableEndTurn = false;
    pgn_state = turnEnd();
    if (pgn_state >= 0) {
      numMoves = 0;
      if (pgn_state % pgn_take == pgn_promotion) {
        Serial.println("promotion");
        return loop_promotion;
      } else {
        setPgnMove();
        if (whosTurn == 1) {
          numTurns++;
        }
        Serial.println("endTurn");
        clock.loop(whosTurn);
        setWhosTurn(whosTurn * (-1));
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
  clock.loop(whosTurn);
  board[reducedMoves[1][1]][reducedMoves[1][2]] = whosTurn * piece;
  setPgnMove();
  setWhosTurn(whosTurn * (-1));
}

// set the red led
void Chess::setRed(boolean on) {
  digitalWrite(redLed, on);
}

void Chess::loadGame() {
  resetSetupBoard();
  lcd->clear();
  //memcpy(board, (short[8][8]){
  //    {0, 0, 0, 0, 0, 0, 0, 0},
  //    {rook, knight, bishop, queen, king, bishop, knight, rook},
  //    {pawn, pawn, pawn, pawn, pawn, pawn, pawn, pawn},
  //    {0, 0, 0, 0, 0, 0, 0, 0},
  //    {0, 0, 0, 0, 0, 0, 0, 0},
  //    {-1*pawn, -1*pawn, -1*pawn, -1*pawn, -1*pawn, -1*pawn, -1*pawn, -1*pawn},
  //    {-1*rook, -1*knight, -1*bishop, -1*queen, -1*king, -1*bishop, -1*knight, -1*rook},
  //    {0, 0, 0, 0, 0, 0, 0, 0}
  //  }, sizeof(board));
  debugBoard(board);
}

boolean Chess::setupBoard() {
  scanBoard(true, false);
  boolean exit = false;
  if (!enableEndTurn && !digitalRead(endTurn)) {
    enableEndTurn = true;
  }
  for (short i = 0; i < nRows; i++) {
    for (short j = 0; j < nRows; j++) {
      short expectedScan = 1;
      if (board[i][j] == 0) {
        if (currScan[i][j] == 1) {
          if (setupPosition[0] != i || setupPosition[1] != j) {
            setupPosition[0] = i;
            setupPosition[1] = j;
            lcd->clearLine();
            lcd->setCursor(1, 0);
            leds->setLEDPattern(j, i);
            lcd->print("no piece");
            lcd->setCursor(13, 0);
            lcd->print(String(cols[j]) + String(i + 1));
          }
          return false;
        }
      } else { // expecting a piece
        if (setupPosition[0] != i || setupPosition[1] != j) {
          if (currScan[i][j] == 0 || (i > setPosition[0] || (i == setPosition[0] && j > setPosition[1]))) {
            forceLoadSelect = true;
            enableEndTurn = false;
            setupPosition[0] = i;
            setupPosition[1] = j;
            lcd->clearLine();
            lcd->setCursor(1, 0);
            leds->setLEDPattern(j, i);
            if (board[i][j] > 0) {
              lcd->print("White ");
            } else {
              lcd->print("Black ");
            }
            lcd->print(String(shortForms[abs(board[i][j])]) + "(");
            lcd->write(byte(abs(board[i][j])));
            lcd->print(")");
            lcd->setCursor(13, 0);
            lcd->print(String(cols[j]) + String(i + 1));
            return false;
          }
        } else {
          if (currScan[i][j] == 0) {
            forceLoadSelect = false;
            return false;
          } else if (forceLoadSelect && (!enableEndTurn || !digitalRead(endTurn))) {
            return false;
          } else if (i > setPosition[0] || (i == setPosition[0] && j > setPosition[1])) {
            setPosition[0] = i;
            setPosition[1] = j;
          }
          enableEndTurn = false;
        }
      }
    }
  }

  resetSetupBoard();
  leds->turnOff();
  memcpy(prevScan, currScan, sizeof(prevScan));
  return true;
}

void Chess::resetSetupBoard() {
  setupPosition[0] = -1;
  setupPosition[1] = -1;
  setPosition[0] = -1;
  setPosition[1] = -1;
  forceLoadSelect = true;
  enableEndTurn = false;
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
          if (!isSlide(diff[i][j], i, j)) {
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
          } else {
            Serial.println("Slide found, moves removed at " + String(cols[j]) + String(i+1));
            Serial.println("NumMoves: " + String(numMoves));
          }
        }
        else {
          // TOO MANY MOVES????
        }
      }
    }
  }
}

boolean Chess::isSlide(short down, short row, short col) {
  if (down == 1) {
    return false;
  }
  boolean slideFound = false;
  boolean move = -1;
  while (!slideFound && move < numMoves) {
    move++;
    if (moves[move][1] == row && moves[move][2] == col && moves[move][0] == 1) {
      slideFound = true;
    }
  }
  
  if (slideFound) {
    numMoves--;
    for (int i=move; i < numMoves; i++) {
      moves[i][0] = moves[i+1][0];
      moves[i][1] = moves[i+1][1];
      moves[i][2] = moves[i+1][2];
    }
    
    return true;
  }
  
  return false;
}

short Chess::turnEnd() {
  kingAttackers[0] = 0;
  kingAttackers[1] = 0;
  Serial.print("turn: ");
  Serial.println(whosTurn);
  Serial.print("numMoves: ");
  Serial.println(numMoves);
  reduceMoves();
  Serial.print("numMoves reduced: ");
  Serial.println(numReducedMoves);
  for (short i=0; i < numReducedMoves; i++) {
    Serial.print("Move ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(reducedMoves[i][0]);
    Serial.print(cols[reducedMoves[i][2]]);
    Serial.println(reducedMoves[i][1]+1);
  }
  
  if (numReducedMoves == 0) {
    lcd->clearLine();
    lcd->print("no moves");
    setRed(false);
    return -1;
  } else if (numReducedMoves == 2) {
    short fromIndex = 0;
    if (reducedMoves[0][1] == -1) {
      fromIndex = 1;
    }
    if (
      reducedMoves[0][0] * reducedMoves[1][0] > 0
      || board[reducedMoves[fromIndex][1]][reducedMoves[fromIndex][2]]*whosTurn <= 0
      || board[reducedMoves[abs(fromIndex - 1)][1]][reducedMoves[abs(fromIndex - 1)][2]]*whosTurn > 0
    ) {
      fixBoard("bad");
      Serial.println("invalid move " + String(board[reducedMoves[0][1]][reducedMoves[0][2]]) + " " + String(whosTurn));
      setRed(true);
      return -1;
    }
	
    short validatorMoves[2][2] =  {{reducedMoves[0][1], reducedMoves[0][2]},{reducedMoves[1][1], reducedMoves[1][2]}};    
    if (!isValidMove(whosTurn*board[reducedMoves[0][1]][reducedMoves[0][2]], validatorMoves)) {
      fixBoard("invalid");
      Serial.println("piece cannot move like so " + String(board[reducedMoves[0][1]][reducedMoves[0][2]]) + " " + String(whosTurn));
      setRed(true);
      return -1;
    }
    
    short kingRow = kingPositions[(whosTurn+1)/2][0];
    short kingCol = kingPositions[(whosTurn+1)/2][1];
    if (whosTurn*board[reducedMoves[0][1]][reducedMoves[0][2]] == 6) {
      kingRow = reducedMoves[1][1];
      kingCol = reducedMoves[1][2];
    }
    
    short checkBoard[8][8];
    for (short i = 0; i < 8; i++) {
      for (short j = 0; j < 8; j++) {
        checkBoard[i][j] = board[i][j];
      }
    }
    checkBoard[reducedMoves[1][1]][reducedMoves[1][2]] =  checkBoard[reducedMoves[0][1]][reducedMoves[0][2]];
    checkBoard[reducedMoves[0][1]][reducedMoves[0][2]] =  0;
    
    if (inCheck(kingRow, kingCol, whosTurn, checkBoard)) {
      lcd->clearLine();
      if (whosTurn < 0) {
        lcd->print("B");
      } else {
        lcd->print("W");
      }
      lcd->print(", in check");
      Serial.println("in check, invalid " + String(board[reducedMoves[0][1]][reducedMoves[0][2]]) + " " + String(whosTurn));
      setRed(true);
      return -1;
    }
    
    boolean checked = inCheck(kingPositions[(whosTurn+2)%3][0], kingPositions[(whosTurn+2)%3][1], -1*whosTurn, checkBoard);
    
    lcd->clearLine();
    if (whosTurn < 0) {
      lcd->print("B");
    } else {
      lcd->print("W");
    }
    debugScan(prevScan);
    lcd->write(byte(whosTurn*board[reducedMoves[0][1]][reducedMoves[0][2]]));
    lcd->print(" ");
    lcd->print(cols[reducedMoves[0][2]]);
    lcd->print(reducedMoves[0][1]+1);
    lcd->print(" to ");
    lcd->print(cols[reducedMoves[1][2]]);
    lcd->print(reducedMoves[1][1]+1);

    short edgeCase = pgn_normal;
    if (board[reducedMoves[1][1]][reducedMoves[1][2]] != 0) {
      edgeCase = pgn_take;
    }
    if (
      reducedMoves[1][1] == (whosTurn + 1) * 7 / 2
      && abs(board[reducedMoves[1][1]][reducedMoves[1][2]]) == pawn
    ) {
      edgeCase += pgn_promotion;
    }

    board[reducedMoves[1][1]][reducedMoves[1][2]] = board[reducedMoves[0][1]][reducedMoves[0][2]];
    board[reducedMoves[0][1]][reducedMoves[0][2]] = 0;
    if (checked) {
      lcd->print(" X");
    }
    if (whosTurn*board[reducedMoves[1][1]][reducedMoves[1][2]] == king) {
      Serial.println("setting King Position");
      kingPositions[(whosTurn+1)/2][0] = reducedMoves[1][1];
      kingPositions[(whosTurn+1)/2][1] = reducedMoves[1][2];
    }
    
    setRed(false);
    Serial.println("edgeCase: " + String(edgeCase));
    return edgeCase;
  } 
  else if (numReducedMoves == 3) {
    Serial.println("Possible Enpassant");
    short row = reducedMoves[0][1];
    short col = reducedMoves[0][2];
    
    if (row == reducedMoves[1][1]){
      Serial.println("Case 2/3");
      if (abs(reducedMoves[1][2] - reducedMoves[0][2]) != 1) {
        Serial.println("too many cells moved");
        fixBoard("bad");
        Serial.println("invalid move " + String(board[reducedMoves[0][1]][reducedMoves[0][2]]) + " " + String(whosTurn));
        setRed(true);
        return -1;
      }
       
    } else if (row == reducedMoves[2][1]) {
      Serial.println("Case 1/4");
      if (
        reducedMoves[2][0] != -1 
        || abs(reducedMoves[2][2] - reducedMoves[0][2]) != 1
        || reducedMoves[1][1] - row != whosTurn
      ) {
        Serial.println("prelim failed");
        fixBoard("bad");
        Serial.println("invalid move " + String(board[reducedMoves[0][1]][reducedMoves[0][2]]) + " " + String(whosTurn));
        setRed(true);
        return -1;
      }
      
      short pawnStart = 0;
      if (col - reducedMoves[1][2] == 0) {
        pawnStart = 2;
      }
      
      Serial.println("pawnStart: " + String(pawnStart));
      
      if (
        board[reducedMoves[pawnStart][1]][reducedMoves[pawnStart][2]]*whosTurn != pawn 
        || board[reducedMoves[2-pawnStart][1]][reducedMoves[2-pawnStart][2]]*whosTurn != pawn*-1
      ) {
        Serial.println("wrong pieces");
        fixBoard("bad");
        Serial.println("invalid move " + String(board[reducedMoves[0][1]][reducedMoves[0][2]]) + " " + String(whosTurn));
        setRed(true);
        return -1;
      }
      
      Serial.println(lastPgnTurn[0]);
      Serial.println(lastPgnTurn[1]);
      if (
        lastPgnTurn[0] == tolower(cols[reducedMoves[2-pawnStart][2]]) 
        && lastPgnTurn[1] == ('1' + reducedMoves[2-pawnStart][1])
      ) {
        Serial.println("En Passant occured");
        short checkBoard[8][8];
        for (short i = 0; i < 8; i++) {
          for (short j = 0; j < 8; j++) {
            checkBoard[i][j] = board[i][j];
          }
        }
        checkBoard[reducedMoves[1][1]][reducedMoves[1][2]] = checkBoard[reducedMoves[0][1]][reducedMoves[0][2]];
        checkBoard[reducedMoves[0][1]][reducedMoves[0][2]] = 0;
        checkBoard[reducedMoves[2][1]][reducedMoves[2][2]] = 0;
        
        if (inCheck(kingPositions[(-1*whosTurn+2)%3][0], kingPositions[(-1*whosTurn+2)%3][1], whosTurn, checkBoard)) {
          lcd->clearLine();
          if (whosTurn < 0) {
            lcd->print("B");
          } else {
            lcd->print("W");
          }
          lcd->print(", in check");
          Serial.println("in check, invalid " + String(board[reducedMoves[0][1]][reducedMoves[0][2]]) + " " + String(whosTurn));
          setRed(true);
          return -1;
        }
        
        boolean checked = inCheck(kingPositions[(whosTurn+2)%3][0], kingPositions[(whosTurn+2)%3][1], -1*whosTurn, checkBoard);
        
        lcd->clearLine();
        if (whosTurn < 0) {
          lcd->print("B");
        } else {
          lcd->print("W");
        }
        debugScan(prevScan);
        lcd->write(byte(whosTurn*board[reducedMoves[0][1]][reducedMoves[0][2]]));
        lcd->print(" ");
        lcd->print(cols[reducedMoves[0][2]]);
        lcd->print(reducedMoves[0][1]+1);
        lcd->print(" to ");
        lcd->print(cols[reducedMoves[1][2]]);
        lcd->print(reducedMoves[1][1]+1);
        if (checked) {
          lcd->print(" X");
        }
        
        setRed(false);
        
        board[reducedMoves[1][1]][reducedMoves[1][2]] = board[reducedMoves[0][1]][reducedMoves[0][2]];
        board[reducedMoves[0][1]][reducedMoves[0][2]] = 0;
        board[reducedMoves[2][1]][reducedMoves[2][2]] = 0;
        return pgn_normal;
      }
      
    } else {
      fixBoard("bad");
      Serial.println("invalid move " + String(board[reducedMoves[0][1]][reducedMoves[0][2]]) + " " + String(whosTurn));
      setRed(true);
      return -1;
    }
  }
  // castling
  else if (numReducedMoves == 4) {
    boolean validCastle = true;
    boolean validKingSide = true;
    boolean validQueenSide = true;
    short castleColumn = (1 - whosTurn) * 7 / 2;
    for (short i = 0; i < numReducedMoves; i++) {
      if (reducedMoves[i][1] != castleColumn) {
        validCastle = false;
        Serial.println("validCastle failed");
      }
      if (reducedMoves[i][0] < 0) {
        if (reducedMoves[i][2] != castlingDepartures[castlingKing][0]
            && reducedMoves[i][2] != castlingDepartures[castlingKing][1])
        {
          validKingSide = false;
        }
        if (reducedMoves[i][2] != castlingDepartures[castlingQueen][0]
            && reducedMoves[i][2] != castlingDepartures[castlingQueen][1])
        {
          validQueenSide = false;
        }
      } else {
        if (reducedMoves[i][2] != castlingArrivals[castlingKing][0]
            && reducedMoves[i][2] != castlingArrivals[castlingKing][1])
        {
          validKingSide = false;
        }
        if (reducedMoves[i][2] != castlingArrivals[castlingQueen][0]
            && reducedMoves[i][2] != castlingArrivals[castlingQueen][1])
        {
          validQueenSide = false;
        }
      }
    }
    if (validCastle && (validKingSide || validQueenSide)) {
      debugScan(prevScan);
      lcd->clearLine();
      if (whosTurn < 0) {
        lcd->print("B");
      } else {
        lcd->print("W");
      }
      lcd->print("O-O");
      setRed(false);
      if (validQueenSide) {
        lcd->print("-O");
        board[castleColumn][castlingArrivals[castlingQueen][0]] = king * whosTurn;
        board[castleColumn][castlingArrivals[castlingQueen][1]] = rook * whosTurn;
        board[castleColumn][castlingDepartures[castlingQueen][0]] = 0;
        board[castleColumn][castlingDepartures[castlingQueen][1]] = 0;
        return pgn_castleQueen;
      } else {
        board[castleColumn][castlingArrivals[castlingKing][0]] = king * whosTurn;
        board[castleColumn][castlingArrivals[castlingKing][1]] = rook * whosTurn;
        board[castleColumn][castlingDepartures[castlingKing][0]] = 0;
        board[castleColumn][castlingDepartures[castlingKing][1]] = 0;
        return pgn_castleKing;
      }
    }
  }
  
  lcd->clearLine();
  lcd->print("too many moves");

  setRed(true);
  return -1;
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
      boolean oppositeTake = false;
      short oppositeTaken = 0;
      for (short i = 0; i < numReduced; i++) {
        // covers resonable cases (still edge cases) for correct taken piece
        if (reduced[i][0] == 0
            && board[reduced[i][1]][reduced[i][2]] * whosTurn < 0)
        {
          oppositeTake = true;
          oppositeTaken = i;
        }
      }
      if (oppositeTake) {
        reduced[oppositeTaken][0] = 1;
      }
    }

    numReducedMoves = 0;
    for (short i = numReduced - 1; i >= 0; i--) {
      if (reduced[i][0] != 0) {
        memcpy(reducedMoves[numReducedMoves], reduced[i], sizeof(reduced[i]));
        numReducedMoves++;
      }
    }  
  }
}

void Chess::debugBoard(short array[8][8]) {
  Serial.println("------------------------");
  Serial.println("    A  B  C  D  E  F  G  H      ");
  for (short i = nRows - 1; i >= 0; i--) {
    Serial.print(String(i + 1) + "   ");
    for (int j = 0; j < nRows; j++) {
      if (array[i][j] > 0) {
        Serial.print(" ");
      } else if (array[i][j] < 0) {
        Serial.print("-");
      } else {
        Serial.print("   ");
      }
      Serial.print(shortForms[abs(array[i][j])]);
    }
    Serial.println("   " + String(i + 1));
  }
  Serial.println("    A  B  C  D  E  F  G  H      ");
}

void Chess::debugCurrentBoard() {
  Serial.println("------------------------");
  Serial.println("    A  B  C  D  E  F  G  H      ");
  for (short i = nRows - 1; i >= 0; i--) {
    Serial.print(String(i + 1) + "   ");
    for (int j = 0; j < nRows; j++) {
      if (board[i][j] > 0) {
        Serial.print(" ");
      } else if (board[i][j] < 0) {
        Serial.print("-");
      } else {
        Serial.print("   ");
      }
      Serial.print(shortForms[abs(board[i][j])]);
    }
    Serial.println("   " + String(i + 1));
  }
  Serial.println("    A  B  C  D  E  F  G  H      ");
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

void Chess::outputBoard(boolean debug) {
  if (debug) {
    debugScan(currScan);
  }
  
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
    lcd->print("nothing...");
  }
}

boolean Chess::isValidMove(short piece, short movesToCheck[2][2]) {
	short rowsMoved = (movesToCheck[1][0] - movesToCheck[0][0])*whosTurn;
	short colsMoved = (movesToCheck[1][1] - movesToCheck[0][1])*whosTurn;
	Serial.println("piece: " + String(piece));
	Serial.println("rowsMoved: " + String(rowsMoved));
	Serial.println("colsMoved: " + String(colsMoved));
	switch (piece) {
		// Pawn
		case 1:
      if (board[movesToCheck[1][0]][movesToCheck[1][1]] != 0) {
        if (colsMoved == 0 || rowsMoved != 1) {
          return false;
        }
      } else {
        // ensure same column
        if (colsMoved != 0) {
          return false;
        }
        // ensure proper number of rows moved
        if (rowsMoved < 1 || rowsMoved > 2) {
          return false;
        }
        // if two rows moved, make sure they started in the starting row
        if (rowsMoved == 2 && movesToCheck[0][0] != (7+whosTurn)%7) {
          return false;
        }
      }
			return true;
		// Knight
		case 2:
			if (abs(colsMoved) > 2 || abs(rowsMoved) > 2) {
				return false;
			} else if (
				(abs(colsMoved) == 2 && abs(rowsMoved) != 1)
				|| (abs(rowsMoved) == 2 && abs(colsMoved) != 1)
			) {
				return false;
			}
			return true;
		// Bishop
		case 3:
			if (abs(colsMoved) != abs(rowsMoved)) {
				return false;
			}
			
			if (!checkCollisions(movesToCheck)) {
				return false;
			}
			
			return true;
		// Rook:
		case 4:
			if (colsMoved != 0 && rowsMoved != 0) {
				return false;
			}
			
			if (!checkCollisions(movesToCheck)) {
				return false;
			}
			
			return true;
		// Queen;
		case 5:
			if ((colsMoved != 0 && rowsMoved != 0) && abs(colsMoved) != abs(rowsMoved)) {
				return false;
			}
			
			if (!checkCollisions(movesToCheck)) {
				return false;
			}
			
			return true;
		// King
		case 6:
			if (abs(colsMoved) > 1 || abs(rowsMoved) > 1) {
				return false;
			} else if (colsMoved != 0 && rowsMoved != 0 && abs(colsMoved) != abs(rowsMoved)) {
				return false;
			}
			return true;
		default:
			return true;
	}
}

boolean Chess::checkCollisions (short movesToCheck[2][2]) {
	short rowsMoved = (movesToCheck[1][0] - movesToCheck[0][0]);
	short colsMoved = (movesToCheck[1][1] - movesToCheck[0][1]);
	short rowDirection = (rowsMoved > 0) - (rowsMoved < 0);
	short colDirection = (colsMoved > 0) - (colsMoved < 0);
	Serial.println("Cells Checked for Collision:");
	if (rowsMoved == 0) {
		for (int i = movesToCheck[0][1] + colDirection; i != movesToCheck[1][1]; i += colDirection) {
			Serial.println("\t" + cols[i] + String(movesToCheck[0][0]));
			if (board[movesToCheck[0][0]][i] != 0) {
				return false;
			}
		}
	} else if (colsMoved == 0) {
		for (int i = movesToCheck[0][0] + rowDirection; i != movesToCheck[1][0]; i += rowDirection) {
			Serial.println("\t" + cols[movesToCheck[0][1]] + String(i));
			if (board[i][movesToCheck[0][1]] != 0) {
				return false;
			}
		}
	} else {
		int j = movesToCheck[0][1] + colDirection;
		for (int i = movesToCheck[0][0] + rowDirection; i != movesToCheck[1][0]; i += rowDirection) {
			Serial.println("\t" + cols[j] + String(i));
			if (board[i][j] != 0) {
				return false;
			}
			j += colDirection;
		}
	}
	
	return true;
}

boolean Chess::inCheck(short kingRow, short kingCol, short kingColour, short checkBoard[8][8]) {
	if (checkBoard == NULL) {
    Serial.println("checkBoard is NULL");
    memcpy(checkBoard, board, sizeof(board));
  }
  
  Serial.println("Checking for Check");
  Serial.println("knigRow: " + String(kingRow));
  Serial.println("kingCol: " + String(kingCol));
  
  short col;
	short row;
	// One who is in check
	short victim = (kingColour+1)/2;
	boolean ownPiece;
	for (short colDir = -1; colDir < 2; colDir += 2) {
		for (short rowDir = -1; rowDir < 2; rowDir += 2) {
      Serial.println("ColDir: " + String(colDir));
      Serial.println("RowDir: " + String(rowDir));
			col = kingCol + colDir;
			row = kingRow + rowDir;
			ownPiece = false;
			// queen or bishop
      Serial.println("Queen or Bishop");
			while (col >= 0 && col < 8 && row >= 0 && row < 8 && !ownPiece) {        
        Serial.println("Row: " + String(row));
        Serial.println("Col: " + String(col));
				if (checkBoard[row][col] != 0) {
          Serial.println("checkBoard: " + String(sign(checkBoard[row][col])));
					if (sign(checkBoard[row][col]) == kingColour) {
						ownPiece = true;
					} else if (checkBoard[row][col] == -1*queen*kingColour || checkBoard[row][col] == -1*bishop*kingColour){
						Serial.println("Attacker Found");
            kingAttackers[victim]++;
					} else {
            ownPiece = true;
          }
				}
				col += colDir;
				row += rowDir;
			}
			
			col = kingCol + (colDir + rowDir)/2;
			row = kingRow + (colDir + -1*rowDir)/2;
			ownPiece = false;
			// queen or rook
      Serial.println("Queen or Rook");
			while (col >= 0 && col < 8 && row >= 0 && row < 8 && !ownPiece) {
        Serial.println("Row: " + String(row));
        Serial.println("Col: " + String(col));
				if (checkBoard[row][col] != 0) {
          Serial.println("checkBoard: " + String(checkBoard[row][col]));
					if (sign(checkBoard[row][col]) == kingColour) {
						ownPiece = true;
					} else if (checkBoard[row][col] == -1*queen*kingColour || checkBoard[row][col] == -1*rook*kingColour) {
						Serial.println("Attacker Found");
            kingAttackers[victim]++;
					} else {
            ownPiece = true;
          }
				}
				col += (colDir + rowDir)/2;
				row += (colDir + -1*rowDir)/2;
			}
			// knight
      Serial.println("Knight");
			row = kingRow + rowDir;
			col = kingCol + 2*colDir;
      Serial.println("Row: " + String(row));
      Serial.println("Col: " + String(col));
			if (row < 8 && row >= 0 && col < 8 && col >= 0 && checkBoard[row][col] == -1*knight*kingColour) {
				Serial.println("checkBoard: " + String(checkBoard[row][col]));
        Serial.println("knightVal: " + String(-2*kingColour));
        Serial.println("Attacker Found");
        kingAttackers[victim]++;
			}
			
			row = kingRow + 2*rowDir;
			col = kingCol + colDir;
      Serial.println("Row: " + String(row));
      Serial.println("Col: " + String(col));
			if (row < 8 && row >= 0 && col < 8 && col >= 0 && checkBoard[row][col] == -1*knight*kingColour) {
				Serial.println("checkBoard: " + String(checkBoard[row][col]));
        Serial.println("knightVal: " + String(-2*kingColour));
        Serial.println("Attacker Found");
        kingAttackers[victim]++;
			}
      // king
      Serial.println("King");
      col = kingCol + (colDir + rowDir)/2;
			row = kingRow + (colDir + -1*rowDir)/2;
      if (row < 8 && row >= 0 && col < 8 && col >= 0 && checkBoard[row][col] == -1*king*kingColour) {
				Serial.println("checkBoard: " + String(checkBoard[row][col]));
        Serial.println("Attacker Found");
        kingAttackers[victim]++;
			}
      
      col = kingCol + colDir;
			row = kingRow + rowDir;
      if (row < 8 && row >= 0 && col < 8 && col >= 0 && checkBoard[row][col] == -1*king*kingColour) {
				Serial.println("checkBoard: " + String(checkBoard[row][col]));
        Serial.println("Attacker Found");
        kingAttackers[victim]++;
			}
		}		
		// Pawn
    Serial.println("Pawn");
		row = kingRow + kingColour;
		col = kingCol + colDir;    
    Serial.println("Row: " + String(row));
    Serial.println("Col: " + String(col));
		if (row < 8 && row >= 0 && col < 8 && col >= 0 && checkBoard[row][col] == -1*pawn*kingColour) {
			Serial.println("checkBoard: " + String(checkBoard[row][col]));
      Serial.println("Attacker Found");
      kingAttackers[victim]++;
		}
	}

	if (kingAttackers[victim] > 0) {
		return true;
	}
	
	return false;
}


  // boolean inCheckmate(short kingRow, short kingCol, short kingColour, short checkBoard[8][8]) {
    // short row;
    // short col;
    // for (short colDir = -1; colDir < 2; colDir += 2) {
      // for (short rowDir = -1; rowDir < 2; rowDir += 2) {
        // col = kingCol + (colDir + rowDir)/2;
        // row = kingRow + (colDir + -1*rowDir)/2;
        // if (row < 8 && row >= 0 && col < 8 && col >= 0 && checkBoard[row][col] == 0) {
          // checkBoard[kingRow][KingCol] = 0;
          // checkBoard[row][col] = king*kingColour;
          // if (!inCheck(row,col, kingColour, checkBoard));
        // }
      // }
    // }
    
    // return true;
  // }

short Chess::sign(short val) {
  if (val > 0) {
    return 1;
  } else if (val < 0) {
    return -1;
  }
  return 0;
}

void Chess::resetFixes() {
  nFixes = 0;
  for (short i = 0; i < 3; i++) {
    for (short j = 0; j < 2; j++) {
      fix[i][j] = -1;
    }
  }
}

// output squares to lcd that are different than what's expected based on board
void Chess::fixBoard(String message, short lcdRow) {
  short nPositions = (16 - message.length()) / 3;
  short nWrong = 0;
  boolean output = false;
  for (short i = 0; i < nRows && nWrong < nPositions; i++) {
    for (short j = 0; j < nRows && nWrong < nPositions; j++) {
      short activated = 1;
      if (board[i][j] == 0) {
        activated = 0;
      }
      if (activated != currScan[i][j]) {
        if (i != fix[nWrong][0] || j != fix[nWrong][1]) {
          fix[nWrong][0] = i;
          fix[nWrong][1] = j;
          output = true;
        }
        nWrong++;
      }
    }
  }
  if (nWrong != nFixes || output) {
    lcd->clearLine(lcdRow);
    if (nWrong > 0) {
      lcd->print(message);
      for (short i = 0; i < nWrong; i++) {
        lcd->print(" " + String(cols[fix[i][1]]) + String(fix[i][0] + 1));
      }
    }
  }
  nFixes = nWrong;
}

void Chess::setPgnMove() {
  strcpy(lastPgnTurn, "");
  
  if (pgn_state == pgn_castleKing) {
    strcat(lastPgnTurn,"O-O");
  } else if (pgn_state == pgn_castleQueen) {
    strcat(lastPgnTurn,"O-O-O");
  } else if (numReducedMoves == 2) {
    switch (board[reducedMoves[1][1]][reducedMoves[1][2]]*whosTurn) {
      case 2:
        strcat(lastPgnTurn,"N");
        break;
      case 3:
        strcat(lastPgnTurn,"B");
        break;
      case 4:
        strcat(lastPgnTurn,"R");
        break;
      case 5:
        strcat(lastPgnTurn,"Q");
        break;
      case 6:
        strcat(lastPgnTurn,"K");
        break;
      default:
        strcat(lastPgnTurn,"");
        break;
    }

    if (pgn_state >= pgn_take) {
      strcat(lastPgnTurn,"x");
    }
    
    switch (reducedMoves[1][2]) {
      case 0:
        strcat(lastPgnTurn,"a");
        break;
      case 1:
        strcat(lastPgnTurn,"b");
        break;
      case 2:
        strcat(lastPgnTurn,"c");
        break;
      case 3:
        strcat(lastPgnTurn,"d");
        break;
      case 4:
        strcat(lastPgnTurn,"e");
        break;
      case 5:
        strcat(lastPgnTurn,"f");
        break;
      case 6:
        strcat(lastPgnTurn,"g");
        break;
      case 7:
        strcat(lastPgnTurn,"h");
        break;
      default:
        break;
    }
    
    char column[1] = "";
    strcat(lastPgnTurn, itoa(reducedMoves[1][1] + 1, column, 10));
    
    if (pgn_state % pgn_take == pgn_promotion) {
      strcat(lastPgnTurn, "=");
      switch (board[reducedMoves[1][1]][reducedMoves[1][2]]) {
        case 2: 
          strcat(lastPgnTurn,"N");
          break;
        case 3:
          strcat(lastPgnTurn,"B");
          break;
        case 4:
          strcat(lastPgnTurn,"R");
          break;
        case 5:
          strcat(lastPgnTurn,"Q");
          break;
        case 6:
          strcat(lastPgnTurn,"K");
          break;
        default:
          strcat(lastPgnTurn,"P");
          break;
      }
    }
    
    if (kingAttackers[(-1*whosTurn+1)/2] > 0) {
       strcat(lastPgnTurn,"+");
    }
    
    Serial.println(lastPgnTurn);
  } else if (numReducedMoves == 3) {
    short row;
    short col;
    for (short i = 0; i < 3; i ++) {
      if (reducedMoves[i][0] == 1) {
        row = reducedMoves[i][1];
        col = reducedMoves[i][2];
      }
    }
    strcat(lastPgnTurn, "x");
    
    switch (col) {
      case 0:
        strcat(lastPgnTurn,"a");
        break;
      case 1:
        strcat(lastPgnTurn,"b");
        break;
      case 2:
        strcat(lastPgnTurn,"c");
        break;
      case 3:
        strcat(lastPgnTurn,"d");
        break;
      case 4:
        strcat(lastPgnTurn,"e");
        break;
      case 5:
        strcat(lastPgnTurn,"f");
        break;
      case 6:
        strcat(lastPgnTurn,"g");
        break;
      case 7:
        strcat(lastPgnTurn,"h");
        break;
      default:
        break;
    }
    
    char column[1] = "";
    strcat(lastPgnTurn, itoa(row + 1, column, 10));
    
    if (kingAttackers[(-1*whosTurn+1)/2] > 0) {
       strcat(lastPgnTurn,"+");
    }
  }
  
  if (strlen(lastPgnTurn) > 0) {
    Serial.print("writing to SD");
    File file = SD.open(fileName, FILE_WRITE);
    if (file) {
      Serial.print("opened file");
      char toPrint[20] = "";
      if (whosTurn == 1) {
        char turnChars[3] = "";
        strcpy(toPrint, itoa(numTurns, turnChars, 10));
        strcat(toPrint, ". ");
      }
      strcat(toPrint, lastPgnTurn);
      strcat(toPrint, " ");
      file.print(toPrint);
      file.close();
    } else {
      Serial.println("error opening file for move write");
    }
  }
}

boolean Chess::moveAttacker(short attackRow, short attackCol, short piece, short fromRow, short fromCol) {
  short col;
	short row;
  short turn = sign(piece);

  if (fromRow > 0 && fromCol > 0) {
    if (board[fromRow][fromCol] == piece) {
      board[fromRow][fromCol] = 0;
      board[attackRow][attackCol] = piece;
      return true;
    }
    return false;
  }
  
  switch (turn*piece) {
    boolean blockingPiece;
    short row;
    short col;
    case 1: // pawn
      if (board[attackRow][attackCol] == 0) {
        for (short rowDir = 1; rowDir < 3; rowDir++) {
          row = attackRow - rowDir * turn;
          col = attackCol;
          if (row < 8 && row >= 0 && col < 8 && col >= 0 && board[row][col] == piece) {
            board[row][col] = 0;
            board[attackRow][attackCol] = piece;
            return true;
          }
        }
      } else {
        for (short colDir = -1; colDir < 2; colDir += 2) {
          row = attackRow - turn;
          col = attackCol + colDir;
          if (row < 8 && row >= 0 && col < 8 && col >= 0 && board[row][col] == piece) {
            board[row][col] = 0;
            board[attackRow][attackCol] = piece;
            return true;
          }
        }
      }
      break;
    case 2: // knight
      for (short colDir = -1; colDir < 2; colDir += 2) {
        for (short rowDir = -1; rowDir < 2; rowDir += 2) {
          row = attackRow + rowDir;
          col = attackCol + 2*colDir;
          if (row < 8 && row >= 0 && col < 8 && col >= 0 && board[row][col] == piece) {
            board[row][col] = 0;
            board[attackRow][attackCol] = piece;
            return true;
          }
          
          row = attackRow + 2*rowDir;
          col = attackCol + colDir;
          if (row < 8 && row >= 0 && col < 8 && col >= 0 && board[row][col] == piece) {
            board[row][col] = 0;
            board[attackRow][attackCol] = piece;
            return true;
          }
        }
      }
      break;
    case 3: // bishop
      for (short colDir = -1; colDir < 2; colDir += 2) { 
        for (short rowDir = -1; rowDir < 2; rowDir += 2) {
          col = attackCol + colDir;
          row = attackRow + rowDir;
          blockingPiece = false;
          while (col >= 0 && col < 8 && row >= 0 && row < 8 && !blockingPiece) {
            if (board[row][col] == piece) {
              board[row][col] = 0;
              board[attackRow][attackCol] = piece;
              Serial.println("found bishop");
              return true;
            }  else if (board[row][col] != 0) {
              blockingPiece = true;
            }
            col += colDir;
            row += rowDir;
          }
        }
      }
      Serial.print("No Bishop Found");
      break;
    case 4: // rook
      for (short colDir = -1; colDir < 2; colDir += 2) { 
        for (short rowDir = -1; rowDir < 2; rowDir += 2) {
          col = attackCol + (colDir + rowDir)/2;
          row = attackRow + (colDir + -1*rowDir)/2;
          blockingPiece = false;
          while (col >= 0 && col < 8 && row >= 0 && row < 8 && !blockingPiece) {
            if (board[row][col] == piece) {
              board[row][col] = 0;
              board[attackRow][attackCol] = piece;
              return true;
            } else if (board[row][col] != 0) {
              blockingPiece = true;
            }
            col += (colDir + rowDir)/2;
            row += (colDir + -1*rowDir)/2;
          }
        }
      }
      break;
    case 5: // queen
      for (short colDir = -1; colDir < 2; colDir += 2) { 
        for (short rowDir = -1; rowDir < 2; rowDir += 2) {
          col = attackCol + (colDir + rowDir)/2;
          row = attackRow + (colDir + -1*rowDir)/2;
          blockingPiece = false;
          while (col >= 0 && col < 8 && row >= 0 && row < 8 && !blockingPiece) {
            if (board[row][col] == piece) {
              board[row][col] = 0;
              board[attackRow][attackCol] = piece;
              return true;
            } else if (board[row][col] != 0) {
              blockingPiece = true;
            }
            col += (colDir + rowDir)/2;
            row += (colDir + -1*rowDir)/2;
          }
          
          col = attackCol + colDir;
          row = attackRow + rowDir;
          blockingPiece = false;
          while (col >= 0 && col < 8 && row >= 0 && row < 8 && !blockingPiece) {
            if (board[row][col] == piece) {
              board[row][col] = 0;
              board[attackRow][attackCol] = piece;
              return true;
            }  else if (board[row][col] != 0) {
              blockingPiece = true;
            }
            col += colDir;
            row += rowDir;
          }
        }
      }
      break;
    case 6: // king
      for (short colDir = -1; colDir < 2; colDir += 2) { 
        for (short rowDir = -1; rowDir < 2; rowDir += 2) {
          col = attackCol + (colDir + rowDir)/2;
          row = attackRow + (colDir + -1*rowDir)/2;
          if (row < 8 && row >= 0 && col < 8 && col >= 0 && board[row][col] == piece) {
            board[row][col] = 0;
            board[attackRow][attackCol] = piece;
            return true;
          }
          
          col = attackCol + colDir;
          row = attackRow + rowDir;
          if (row < 8 && row >= 0 && col < 8 && col >= 0 && board[row][col] == piece) {
            board[row][col] = 0;
            board[attackRow][attackCol] = piece;
            return true;
          }
        }
      }
      break;
  }
  
  Serial.println("Shit, this aint working");
  return false;
}

boolean Chess::moveCastle(boolean kingSide) {
  short sideIndex = 0;
  if (!kingSide) {
    sideIndex = 1;
  }
  short row = 0;
  if (whosTurn == -1) {
    row = 7;
  }
  if
  (
    board[row][castlingDepartures[sideIndex][0]] == king * whosTurn
    && board[row][castlingDepartures[sideIndex][1]] == rook * whosTurn
    && board[row][castlingArrivals[sideIndex][0]] == 0
    && board[row][castlingArrivals[sideIndex][1]] == 0
  ) {
    board[row][castlingDepartures[sideIndex][0]] = 0;
    board[row][castlingDepartures[sideIndex][1]] = 0;
    board[row][castlingArrivals[sideIndex][0]] = king * whosTurn;
    board[row][castlingArrivals[sideIndex][1]] = rook * whosTurn;
    return true;
  }
  return false;
}

boolean Chess::movePromotion(short attackCol, short piece) {
  short attackRow = 0;
  if (sign(piece) == 1) {
    attackRow = 7;
  }
  if (moveAttacker(attackRow, attackCol, whosTurn * pawn)) {
    board[attackRow][attackCol] = piece;
    return true;
  }
  return false;
}

boolean Chess::moveEnpassant(short attackRow, short attackCol) {
  short row = attackRow - whosTurn;
  short col = attackCol;
  if (attackRow == 2 || attackRow == 6 && moveAttacker(attackRow, attackCol, whosTurn * pawn)) {
    if (board[row][col] == pawn) {
      board[row][col] = 0;
      return true;
    }
  }
  return false;
}

void Chess::setWhosTurn(short s) {
  if (s < 0) {
    whosTurn = -1;
  } else {
    whosTurn = 1;
  }
  leds->lightWhosTurn(whosTurn);
}