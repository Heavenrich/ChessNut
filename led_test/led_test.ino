boolean ledInput;
int ledControl[7] = {
  44,45,46,47,48,49,50
}; //A-C for row, A-C for column, input voltage

void setup() {
  Serial.begin(9600);

  //turn off LEDs if applicable
  for(int i = 0; i < 7; i++) {
    pinMode(ledControl[i], OUTPUT);
    digitalWrite(ledControl[i],LOW);
  }
  ledInput = false;
  flipLEDs();
}

void loop() {
  for (short i = 0; i < 8; i++) {
    for (short j = 0; j < 8; j++) {
      delay(200);
      Serial.println("row: " + String(i) + " column: " + String(j + 1));
      flipLEDs();
      setLEDPattern(i, j);
      flipLEDs();
    }
  }
}

void setLEDPattern(int row, int column) {
  int tmp[2] = {row, column};
  for(int j = 0; j < 2; j++) {
    int i = j * 3;
    switch(tmp[j]) {
       case 0:
         digitalWrite(ledControl[0+i],LOW);
         digitalWrite(ledControl[1+i],LOW);
         digitalWrite(ledControl[2+i],LOW);
         break;
      case 1:
         digitalWrite(ledControl[0+i],HIGH);
         digitalWrite(ledControl[1+i],LOW);
         digitalWrite(ledControl[2+i],LOW);
         break;
      case 2:
         digitalWrite(ledControl[0+i],LOW);
         digitalWrite(ledControl[1+i],HIGH);
         digitalWrite(ledControl[2+i],LOW);
         break;
      case 3:
         digitalWrite(ledControl[0+i],HIGH);
         digitalWrite(ledControl[1+i],HIGH);
         digitalWrite(ledControl[2+i],LOW);
         break;
      case 4:
         digitalWrite(ledControl[0+i],LOW);
         digitalWrite(ledControl[1+i],LOW);
         digitalWrite(ledControl[2+i],HIGH);
         break;
      case 5:
         digitalWrite(ledControl[0+i],HIGH);
         digitalWrite(ledControl[1+i],LOW);
         digitalWrite(ledControl[2+i],HIGH);
         break;
      case 6:
         digitalWrite(ledControl[0+i],LOW);
         digitalWrite(ledControl[1+i],HIGH);
         digitalWrite(ledControl[2+i],HIGH);
         break;
      case 7:
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
  ledInput = !ledInput;
  digitalWrite(ledControl[6], ledInput);
}
