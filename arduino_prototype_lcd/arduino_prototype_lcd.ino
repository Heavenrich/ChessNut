#include <LiquidCrystal.h>
LiquidCrystal lcd(27, 26, 25, 24, 23, 22);

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
  2,3,4,5,6,7,8,9
};

int shiftInput = 12;
int clock = 10;
int period = 50;
int nRows = 8;
int rowShift;

void setup() {
  for (int i = 0; i < nRows; i++) {
    pinMode(columnShift[i], INPUT);
  }
  pinMode(clock, OUTPUT);
  pinMode(shiftInput, OUTPUT);
  rowShift = 0;
  lcd.begin(16, 2);
}

void loop() {
  clockCycle(HIGH);
  for (int i = 0; i < nRows; i++) {
    clockCycle(LOW);
  }
  lcd.clear();
  lcd.setCursor(1,0);
  int count = 0;
  for (int i = 0; i < nRows; i++) {
     for (int j = 0; j < nRows; j++) {
       if (board[i][j] == 1) {
         lcd.print(rows[i]);
         lcd.print(j+1);
         lcd.print(" ");
         count++;
       }
       if (count == 5) {
         lcd.setCursor(1,1);
       }
    }
  }
}

void clockCycle(boolean i) {
  rowShift = rowShift % nRows + 1;
  digitalWrite(clock, LOW);
  digitalWrite(shiftInput, i);
  delay(period / 2);
  
  digitalWrite(clock, HIGH);
  for (int i = 0; i < nRows; i++) {
    board[rowShift][i] = digitalRead(columnShift[i]);
  }
  delay(period / 2);
}
