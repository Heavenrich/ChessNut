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

char rows[8] = {
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
int scan = 11;
boolean enable = true;

void setup() {
  Serial.begin(9600);
  
  for (int i = 0; i < nRows; i++) {
    pinMode(columnShift[i], INPUT);
    pinMode(i+2, OUTPUT);
  }
  pinMode(scan, INPUT);
  lcd.createChar(1, pawnChar);
  lcd.createChar(2, knightChar);
  lcd.createChar(3, bishopChar);
  lcd.createChar(4, rookChar);
  lcd.createChar(5, queenChar);
  lcd.createChar(6, kingChar);
  lcd.begin(16, 2);
}

// true if a game is underway
boolean gameOn = false;

void loop() {
  if (scanBoard(false)) {
    if (gameOn) {
      calculateMove();
    } else if (initialize()){
        lcd.clear();
        lcd.print("ready to start!");
        gameOn = true;
    }
  }   
}

boolean scanBoard(boolean output) {
  boolean scanned = false;
  if (enable && digitalRead(scan)){
    enable = false;
    memcpy(prevScan, currScan, sizeof(prevScan));
    for (int i = 0; i < nRows; i++) {
      scanRow(i);
    }
    if (output) {
      outputBoard();
    }
    
    scanned = true;
  } else if (!digitalRead(scan)) {
    enable = true;
  }
  
  setDiff();
  
  return scanned;
}

// fill the diff matrix with current - previous scans
void setDiff() {
  for (int i = 0; i < nRows; i++) {
    for (int j = 0; j < nRows; j++) {
      diff[i][j] = currScan[i][j] - prevScan[i][j];
    }
  }  
}

// identify which piece has been moved, update board accordingly, print move to lcd
void calculateMove() {
  int newPiece[2][2];
  int numNew = 0;
  int oldPiece[2][2];
  int numOld = 0;
  
  for (int i=0; i < nRows; i++) {
    for (int j=0; j < nRows; j++) {
      if (diff[i][j] == 1) {
        numNew++;
        if (numNew <= 2) {
          newPiece[numNew-1][0] = i;
          newPiece[numNew-1][1] = j;
        }
      } else if (diff[i][j] == -1) {
        numOld++;
        if (numOld <= 2) {
          oldPiece[numOld-1][0] = i;
          oldPiece[numOld-1][1] = j;
        }
      }
    }
  }
  
  if (numNew == 1 && numOld == 1) {
    lcd.clear();
    int piece = board[oldPiece[0][0]][oldPiece[0][1]];
    if (piece < 0) {
      lcd.print("B");
      lcd.write(byte(-1*piece));
    } else {
      lcd.print("W");
      lcd.write(byte(piece));
    }
    lcd.print(" ");
    lcd.print(rows[oldPiece[0][1]]);
    lcd.print(oldPiece[0][0]+1);
    lcd.print(" to ");
    lcd.print(rows[newPiece[0][1]]);
    lcd.print(newPiece[0][0]+1);
    board[newPiece[0][0]][newPiece[0][1]] = board[oldPiece[0][0]][oldPiece[0][1]];
    board[oldPiece[0][0]][oldPiece[0][1]] = 0;   
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
        lcd.print(rows[j]);
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
  digitalWrite(row + 2, HIGH);
  for (int i = 0; i < nRows; i++) {
    if (i != row) {
      digitalWrite(i+2, LOW);
    }
  }
  
  delay(period);
  
  for (int i = 0; i < nRows; i++) {
    currScan[row][i] = digitalRead(columnShift[i]);
  }
}

void promotePawn() {
  if (enable && digitalRead(scan)){
    iPromotion = (iPromotion + 1) % nPromotions;
    enable = false;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(promotions[iPromotion].statement + " ");
    lcd.write(byte(promotions[iPromotion].symbol));
  } else if (!digitalRead(scan)) {
    enable = true;
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
