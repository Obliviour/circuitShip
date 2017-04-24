#include <Key.h>
#include <Keypad.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

void waitForStart();
void setUp();
void findStart(int len);
boolean noConflict(int x, int y, int len, int orien);

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {8, 7, 6, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 3, 2}; //connect to the column pinouts of the keypad

int primaryBoard[8][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
};

int secondaryBoard[8][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
};

int coord[2] = {0, 0};
int endCoord[2] = {0, 0};

Adafruit_BicolorMatrix matrix1 = Adafruit_BicolorMatrix();
Adafruit_BicolorMatrix matrix2 = Adafruit_BicolorMatrix();


Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  Serial.begin(9600);
  Serial.println("8x8 LED Matrix Test");
  
  matrix1.begin(0x70);  // pass in the address
  matrix2.begin(0x71);  // pass in the address  
}

int shipLengths[4] = {2, 3, 3, 4};

void loop() {
  waitForStart();
  shipSetUp();
}

void waitForStart() {
    matrix1.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
    matrix1.setTextSize(1);
    matrix1.setTextColor(LED_GREEN);
    matrix2.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
    matrix2.setTextSize(1);
    matrix2.setTextColor(LED_RED);
    for (int8_t x=7; x>=-36; x--) {
      char in = keypad.getKey();
      if (in != 0) {
        break;
      }
      matrix2.clear();
      matrix2.setCursor(x,0);
      matrix2.print("BATTLE");
      matrix2.writeDisplay();
      matrix1.clear();
      matrix1.setCursor(x,0);
      matrix1.print("SHIP!!");
      matrix1.writeDisplay();
      delay(100);
      if (x == -36) {
        x = 8;
      }
  }
}

void shipSetUp() {
  matrix2.clear();
  matrix2.writeDisplay();
  matrix1.clear();
  matrix1.writeDisplay();
  int numShips = sizeof(shipLengths)/sizeof(shipLengths[0]);
  for (int i = 0; i < numShips; i++) {
    int len = shipLengths[i];
    findStart(len);  //This guy calls noconflict which clears one of the ships
    drawOnes(); //This draws all the ships on the board
    boolean done = false;
    int orientation = 0;
    delay(100);
    while (!done) {
      matrix1.drawLine(coord[0], coord[1], endCoord[0], endCoord[1], LED_YELLOW);
      matrix1.writeDisplay(); 
      char in = keypad.getKey();
      if (in != 0) {
        if (in == '2') {
          noConflict(coord[0], coord[1] - 1, len, orientation);
        } else if (in == '6') {
          noConflict(coord[0] + 1, coord[1], len, orientation);
        } else if (in == '8') {
          noConflict(coord[0], coord[1] + 1, len, orientation);
        } else if (in == '4') {
          noConflict(coord[0] - 1, coord[1], len, orientation);
        } else if (in == '3') {
          if (noConflict(coord[0], coord[1], len, (orientation + 1)%4)) {
            orientation = (orientation + 1)%4;
          }
        } else if (in == '1') {
          if (noConflict(coord[0], coord[1], len, (orientation + 3)%4)) {
            orientation = (orientation + 3)%4;
          }
        } else if (in == '5')  {
          int sX = coord[0];
          int sY = coord[1];
          coord[0] = endCoord[0];
          coord[1] = endCoord[1];
          endCoord[0] = sX;
          endCoord[1] = sY;
          orientation = (orientation + 2) % 4;
        } else if (in == '*' || in == '#') {
          done = true;
          if (orientation == 0 || orientation == 1) {
           for (int j = coord[0]; j <= endCoord[0]; j++) {
            for (int w = coord[1]; w <= endCoord[1]; w++) {
              primaryBoard[w][j] = 1; 
            }
           }
          } else {
           for (int j = coord[0]; j >= endCoord[0]; j--) {
            for (int w = coord[1]; w >= endCoord[1]; w--) {
              primaryBoard[w][j] = 1; 
            }
           }
          }
          //drawOnes();
        }
      }
    }
  }
}


void findStart(int len) {
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      if (noConflict(x, y, len, 0)) {
        return;
      }
    }
  }
}

boolean noConflict(int x, int y, int len, int orien) {
  if (x < 0 || x > 7 || y < 0 || y > 7) {
    return false;
  }
  int sum = 0;
  for (int k = 0; k <=  len; k++) {
    if (orien == 0 && (x+k) < 8) {
      sum += primaryBoard[y][x+k]; 
    } else if (orien == 1 && (y+k) < 8) {
      sum += primaryBoard[y+k][x];
    } else if (orien == 2 && (x-k) >= 0) {
      sum += primaryBoard[y][x-k];
    } else if (orien == 3 && (y-k) >= 0) {
      sum += primaryBoard[y-k][x];
    } else {
      return false;
    }
  }
  if (sum == 0) {
    matrix1.drawLine(coord[0],coord[1], endCoord[0], endCoord[1], 0);
    matrix1.writeDisplay();
    coord[0] = x;
    coord[1] = y;
    if (orien == 0) {
      endCoord[0] = x + len;
      endCoord[1] = y;
    } else if (orien == 1) {
      endCoord[0] = x;
      endCoord[1] = y + len;
    } else if (orien == 2) {
      endCoord[0] = x - len;
      endCoord[1] = y;
    } else {
      endCoord[0] = x;
      endCoord[1] = y - len;
    }
    return true;
  }
  return false;  
}

void drawOnes() {
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      Serial.print(primaryBoard[y][x]);
      if (primaryBoard[y][x] == 1) {
        matrix1.drawPixel(x, y, LED_RED);
        matrix1.writeDisplay();
      }
    }
    Serial.println();
  }
}

