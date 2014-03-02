int led1 = 3;
int led2 = 4;
int led3 = 1;
int led4 = 2;
int c1Shift = 5;
int c2Shift = 6;
int shiftInput = 12;
int clock = 10;
int power = 11;
int period = 50;
int nRows = 2;
int rowShift;

void setup() {
  pinMode(c1Shift, INPUT);
  pinMode(c2Shift, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(power, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(shiftInput, OUTPUT);
  digitalWrite(power, HIGH);
  rowShift = 0;
}

void loop() {
  clockCycle(HIGH);
  for (int i = 1; i < nRows; i++) {
    clockCycle(LOW);
  }
}

void clockCycle(boolean i) {
  rowShift = rowShift % nRows + 1;
  digitalWrite(clock, LOW);
  digitalWrite(shiftInput, i);
  delay(period / 2);
  
  digitalWrite(clock, HIGH);
  if (rowShift == 1) {
    digitalWrite(led1, digitalRead(c1Shift));
    digitalWrite(led2, digitalRead(c2Shift));
  }
  if (rowShift == 2) {
    digitalWrite(led3, digitalRead(c1Shift));
    digitalWrite(led4, digitalRead(c2Shift));
  }
  delay(period / 2);
}
