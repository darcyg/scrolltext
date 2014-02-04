#include "HT1632.h"

#define DATA 2
#define WR   3
#define CS   4
#define CS2  5
#define CS3  6
#define CS4  7

#define PANELS 4
#define CHARWIDTH 12
#define SCREENWIDTH 24 * PANELS
#define SCREENHEIGHT 16 * PANELS
#define DELAYTIME 15

HT1632LEDMatrix matrix = HT1632LEDMatrix(DATA, WR, CS, CS2, CS3, CS4);

String str = "";

void setup() {
  Serial.begin(9600);
  matrix.begin(HT1632_COMMON_16NMOS);  
  matrix.fillScreen();
  delay(500);
  matrix.clearScreen(); 
  matrix.setTextSize(2);
  matrix.setTextColor(1);
}

void scrollText3(String str, int delayTime) {
  int posInString = 0;        // the index of the character being written
  String charToWrite = "";    // the character to be written
  int scrollCounter = 0;      // how many pixels have been shifted while writing the current character
  int timesScrolled = 0;      // total number of pixels shifted. when this hits the limit (maxTimedScrolled), loop is done
  
  // number of times to shift pixels: CHARWIDTH to scroll the first
  // character from the buffer to the last panel,
  // then SCREENWIDTH to scroll the initial character off the board,
  // then CHARWIDTH for each of the remaining characters
  int maxTimesScrolled = SCREENWIDTH + CHARWIDTH * (str.length());

  while (timesScrolled < maxTimesScrolled) {
    if (scrollCounter == 0) {
      // ready to write next character just outside the right boundary
      charToWrite = str.substring(posInString, posInString + 1);
      matrix.setCursor(SCREENWIDTH, 0);
      matrix.print(charToWrite);
    }

    matrix.writeScreen();
    
    delay(delayTime);
    matrix.shiftLeft();
    
    scrollCounter++;
    timesScrolled++;
    if (scrollCounter == CHARWIDTH && posInString < str.length() - 1) {
      scrollCounter = 0;
      posInString++;
    }
  }
}

void loop() {
  char incomingByte = 0;
  if (Serial.available() > 0) {
    incomingByte = (char)Serial.read();
    if ((int)incomingByte != 10) {
      str += incomingByte;
    }
    else if ((int)incomingByte == 10) {
      scrollText3(str, DELAYTIME);
      Serial.println(str);
      str = "";
    }
  } 
}

