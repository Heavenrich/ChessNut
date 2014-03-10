int ledControl[7] = {
  44,45,46,47,48,49,50
}; //A-C for row, A-C for column, input voltage

void setup() {
  //turn off LEDs if applicable
  for(int i = 0; i < 7; i++) {
    digitalWrite(ledControl[i],LOW);
  }
}

void loop() {
  for(int row = 1; row < 9; row++) {
    for(int column = 1; column < 9; column++) {
      setLEDPattern(row,column);
      delay(100);
    }
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
