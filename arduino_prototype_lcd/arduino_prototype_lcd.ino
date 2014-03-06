#include <LiquidCrystal.h>
LiquidCrystal lcd(27, 26, 25, 24, 23, 22);

byte pawn[8] = {
  B00000,
  B00000,
  B00000,
  B01100,
  B01100,
  B01100,
  B01100,
  B11110
};

byte knight[8] = {
  B00000,
  B01000,
  B01110,
  B01111,
  B01111,
  B01100,
  B01100,
  B11110
};

byte bishop[8] = {
  B00000,
  B00100,
  B01100,
  B01000,
  B01100,
  B01100,
  B01100,
  B11110
};

byte rook[8] = {
  B00000,
  B00000,
  B10101,
  B11111,
  B01110,
  B01110,
  B01110,
  B11111
};

byte king[8] = {
  B00100,
  B01110,
  B00100,
  B01110,
  B01110,
  B01110,
  B01110,
  B11111
};

byte queen[8] = {
  B00100,
  B01110,
  B01110,
  B01110,
  B01110,
  B01110,
  B01110,
  B11111
};


int board[8][8] = {
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

int shiftInput = 12;
int clock = 10;
int quarterPeriod = 5;
int nRows = 8;
int rowShift;

void setup() {
  for (int i = 0; i < nRows; i++) {
    pinMode(columnShift[i], INPUT);
  }
  pinMode(clock, OUTPUT);
  pinMode(shiftInput, OUTPUT);
  rowShift = nRows - 1;
  lcd.createChar(0, pawn);
  lcd.createChar(1, knight);
  lcd.createChar(2, bishop);
  lcd.createChar(3, rook);
  lcd.createChar(4, king);
  lcd.createChar(5, queen);
  lcd.begin(16, 2);
}

void loop() {
  clockCycle(HIGH);
  for (int i = 1; i < nRows; i++) {
    clockCycle(LOW);
  }
  lcd.clear();
  lcd.setCursor(1,0);
  int count = 0;
  for (int i = 0; i < nRows; i++) {
     for (int j = 0; j < nRows; j++) {
       if (board[i][j] == 1) {
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

void clockCycle(boolean i) {
  rowShift = (rowShift + 1) % nRows;
  digitalWrite(clock, LOW);
  digitalWrite(shiftInput, i);
  delay(2 * quarterPeriod);
  
  digitalWrite(clock, HIGH);
  delay(quarterPeriod);
  
  for (int i = 0; i < nRows; i++) {
    board[rowShift][i] = digitalRead(columnShift[i]);
  }
  delay(quarterPeriod);
}
