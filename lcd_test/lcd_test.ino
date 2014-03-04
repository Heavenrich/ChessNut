// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
//LiquidCryst lcd(12, 11, 5, 4, 3, 2);
LiquidCrystal lcd(27, 26, 25, 24, 23, 22);

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Science Bitch!");
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  delay(500);
  // print the number of seconds since reset:
  lcd.print(millis()/1000);
}
