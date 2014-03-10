#include <LiquidCrystal.h>
#include <Promotion.h>

LiquidCrystal lcd(27, 26, 25, 24, 23, 22);

short pawn = 1;
byte pawnChar[8] = {
  B00000,
  B00000,
  B00000,
  B01100,
  B01100,
  B01100,
  B01100,
  B11110
};

short knight = 2;
byte knightChar[8] = {
  B00000,
  B01000,
  B01110,
  B01111,
  B01111,
  B01100,
  B01100,
  B11110
};

short bishop = 3;
byte bishopChar[8] = {
  B00000,
  B00100,
  B01100,
  B01000,
  B01100,
  B01100,
  B01100,
  B11110
};

short rook = 4;
byte rookChar[8] = {
  B00000,
  B00000,
  B10101,
  B11111,
  B01110,
  B01110,
  B01110,
  B11111
};

short queen = 5;
byte queenChar[8] = {
  B00100,
  B01110,
  B01110,
  B01110,
  B01110,
  B01110,
  B01110,
  B11111
};

short king = 6;
byte kingChar[8] = {
  B00100,
  B01110,
  B00100,
  B01110,
  B01110,
  B01110,
  B01110,
  B11111
};

int prevScan[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};

int currScan[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};

int diff[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};

short board[8][8] = {
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

int columnShift[8] = {
  35,34,33,32,31,30,29,28
};

Promotion promotions[2] = {
  Promotion(String("Promote to Q?"), 5),
  Promotion(String("Promote to N?"), 1)
};
int nPromotions = 2;
int iPromotion = 0;

int period = 50;
int nRows = 8;
int endTurn = 11;
boolean enableEndTurn = true;
int scan = 12;
boolean enableScan = true;

int ledControl[7] = {
  44,45,46,47,48,49,50
}; //A-C for row, A-C for column, input voltage

void setup() {
  Serial.begin(9600);
  
  for (int i = 0; i < nRows; i++) {
    pinMode(columnShift[i], INPUT);
    pinMode(i+36, OUTPUT);
  }
  pinMode(endTurn, INPUT);
  pinMode(scan, INPUT);
  lcd.createChar(1, pawnChar);
  lcd.createChar(2, knightChar);
  lcd.createChar(3, bishopChar);
  lcd.createChar(4, rookChar);
  lcd.createChar(5, queenChar);
  lcd.createChar(6, kingChar);
  lcd.begin(16, 2);
  
  //turn off LEDs if applicable
  for(int i = 0; i < 7; i++) {
    digitalWrite(ledControl[i],LOW);
  }
  
}

// true if a game is underway
boolean gameOn = false;

// if 1, then white's turn, if -1 then black's turn
short whosTurn = true;

// list of moves made this turn 
// moves[0] = {-1, 2, 4} means the first move of the turn was a piece being removed from D2
// moves[1] = {1, 4, 4} means the second move of the turn was a piece being placed at D4
#define MAX_MOVES 20
int moves[MAX_MOVES][3];
int numMoves = 0;

void loop() {
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
    if (gameOn) {
      detectMove();
    } else if (initialize()){
        lcd.clear();
        lcd.print("ready to start!");
        gameOn = true;
        memcpy(prevScan, currScan, sizeof(prevScan));
    }
  }
}

boolean scanBoard(boolean output, boolean continuous) {
  boolean scanned = false;
  if ((enableScan && digitalRead(scan)) || continuous){
    enableScan = false;
    // memcpy(prevScan, currScan, sizeof(prevScan));
    for (int i = 0; i < nRows; i++) {
      scanRow(i);
    }
    if (output) {
      outputBoard();
    }
    
    scanned = true;
  } else if (!digitalRead(scan)) {
    enableScan = true;
  }
  
  setDiff();
  
  return scanned;
}

boolean turnEnd() {
  Serial.print("\nturn: ");
  Serial.print(whosTurn);
  Serial.print("\nnumMoves: ");
  Serial.print(numMoves);
  for (int i=0; i < 4; i++) {
    Serial.print("\nMove ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(moves[i][0]);
    Serial.print(cols[moves[i][2]]);
    Serial.print(moves[i][1]+1);
  }
  reduceMoves();
  Serial.print("\nnumMoves reduced: ");
  Serial.print(numMoves);
  for (int i=0; i < 4; i++) {
    Serial.print("\nMove ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(moves[i][0]);
    Serial.print(cols[moves[i][2]]);
    Serial.print(moves[i][1]+1);
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
    Serial.print("------------\n");
    Serial.print("  ABCDEFGH  \n");
    for (int i = nRows - 1; i >= 0; i--) {
      Serial.print(String(i + 1) + " ");
      for (int j = 0; j < nRows; j++) {
        Serial.print(prevScan[i][j]);
      }
      Serial.print(" " + String(i + 1) + "\n");
    }
    Serial.print("  ABCDEFGH  \n");
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
    Serial.print("------------\n");
    Serial.print("  ABCDEFGH  \n");
    for (int i = nRows - 1; i >= 0; i--) {
      Serial.print(String(i + 1) + " ");
      for (int j = 0; j < nRows; j++) {
        Serial.print(prevScan[i][j]);
      }
      Serial.print(" " + String(i + 1) + "\n");
    }
    Serial.print("  ABCDEFGH  \n");
    
    return true;
  }
  
  lcd.clear();
  lcd.print("too many moves");
  return false;
}

void reduceMoves() {
  int numReduced = 0;
  int numUp = 0;
  int reduced[MAX_MOVES][3];
  for (int i=0; i < numMoves; i++) {
    if (moves[i][0] == -1) {
      numUp++;
    } else {
      numUp--;
    }
    if (
      moves[i][0] == -1
      && i+2 < numMoves
      && moves[i+1][0] == 1
      && moves[i+2][0] == -1
      && moves[i+1][1] == moves[i+2][1]
      && moves[i+1][2] == moves[i+2][2]
    ) {
      reduced[numReduced][0] = -1;
      reduced[numReduced][1] = moves[i][1];
      reduced[numReduced][2] = moves[i][2];
      numReduced++;
      i+=2;
    } else {
      reduced[numReduced][0] = moves[i][0];
      reduced[numReduced][1] = moves[i][1];
      reduced[numReduced][2] = moves[i][2];
      numReduced++;
    }
  }
  
  numMoves = numReduced;
  
  memcpy(moves, reduced, sizeof(int)*3*numReduced);
}

// fill the diff matrix with current - previous scans
void setDiff() {
  for (int i = 0; i < nRows; i++) {
    for (int j = 0; j < nRows; j++) {
      diff[i][j] = currScan[i][j] - prevScan[i][j];
    }
  }  
}

// analyze diff and save new moves made
void detectMove() {
  boolean made;
  short state;
  int relativeDiff;
  for (int i=0; i < nRows; i++) {
    for (int j=0; j < nRows; j++) {
      state = prevScan[i][j];
      for (int k = 0; k < numMoves; k++) {
        if (moves[k][1] == i && moves[k][2] == j) {
          state += moves[k][0];
        }
      }
      relativeDiff = state - prevScan[i][j];
      Serial.print("\n");
      Serial.print(cols[j]);
      Serial.print(i+1);
      Serial.print("\trelativeDiff: ");
      Serial.print(relativeDiff);
      Serial.print("\tdiff: ");
      Serial.print(diff[i][j]);
      if (relativeDiff - diff[i][j] != 0) {
        if (numMoves < MAX_MOVES) {
          numMoves++;
          moves[numMoves-1][0] = diff[i][j] - relativeDiff;
          moves[numMoves-1][1] = i;
          moves[numMoves-1][2] = j;
        }
        else {
          // TOO MANY MOVES????
        }
      }
    }
  }
}

void outputBoard() {
  Serial.print("------------\n");
  Serial.print("  ABCDEFGH  \n");
  for (int i = nRows - 1; i >= 0; i--) {
    Serial.print(String(i + 1) + " ");
    for (int j = 0; j < nRows; j++) {
      Serial.print(currScan[i][j]);
    }
    Serial.print(" " + String(i + 1) + "\n");
  }
  Serial.print("  ABCDEFGH  \n");
  
  lcd.clear();
  lcd.setCursor(1,0);
  int count = 0;
  for (int i = 0; i < nRows; i++) {
    for (int j = 0; j < nRows; j++) {
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


void scanRow(int row) {
  digitalWrite(row + 36, HIGH);
  for (int i = 0; i < nRows; i++) {
    if (i != row) {
      digitalWrite(i+36, LOW);
    }
  }
  
  delay(period);
  
  for (int i = 0; i < nRows; i++) {
    currScan[row][i] = digitalRead(columnShift[i]);
  }
}

void promotePawn() {
  if (enableScan && digitalRead(endTurn)){
    iPromotion = (iPromotion + 1) % nPromotions;
    enableScan = false;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(promotions[iPromotion].statement + " ");
    lcd.write(byte(promotions[iPromotion].symbol));
  } else if (!digitalRead(endTurn)) {
    enableScan = true;
  }
}

// determine if the board has been set up in the standard starting position
// if it has, then initialize the board matrix and return true
// else return false
boolean initialize() {
  boolean initialized = true;
  for (short i=0; i < nRows; i ++) {
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
