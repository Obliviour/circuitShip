

#include <Key.h>
#include <Keypad.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

void waitForStart();
void setUp();
void drawPrimOnes();
void drawTracOnes();
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

boolean otherSetUp = false;
boolean gameOver = false;
int otherGuess[2] = {0, 0};
int myGuess[2] = {0, 0};
int trans_value = 0;
int read_value = 0;
int time = 0;
boolean winner = false;

const int len = 2;
char sentValue[len]; 
int index = 0;  
int xpos_speech=0;
int ypos_speech=0;
int eventCalled = 0;

Adafruit_BicolorMatrix matrix1 = Adafruit_BicolorMatrix();
Adafruit_BicolorMatrix matrix2 = Adafruit_BicolorMatrix();


Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS );
Keypad keypad2 = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  Serial.begin(9600);
  Serial.println("8x8 LED Matrix Test");
  
  matrix1.begin(0x70);  // pass in the address
  matrix2.begin(0x71);  // pass in the address

  Wire.begin(10);
}

int shipLengths[4] = {2, 3, 3, 4};

void loop() {    

  //starts in read mode
  waitForStart();
  shipSetUp();
  drawPrimOnes();
  trans_value = 1;
  while (!otherSetUp) {
    delay(10);
    Wire.begin(10); 
    Wire.requestFrom(20, 1);    // request 1 bytes from slave device #20
    read_value = Wire.read();
    if (read_value == 1) {
      otherSetUp = true;
    }
    Wire.begin(10);
    Wire.onRequest(requestEvent);
    delay(20);
  }
  delay(10);
  //change to write mode

  matrix2.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix2.setTextSize(1);
  matrix2.setTextColor(LED_RED);
  for (int8_t x=7; x>=-36; x--) {
    matrix2.clear();
    matrix2.setCursor(x,0);
    matrix2.print("BEGIN!");
    matrix2.writeDisplay();
    delay(100);
  }
  
  while (!gameOver) {
    trans_value = 0;
    Wire.begin(10);
    Wire.onRequest(requestEvent); // register event
    
    makeGuess();

    trans_value = myGuess[0]*10 + myGuess[1];
    time = 0;
    while(time < 10) {
      Wire.begin(10); 
      Wire.onRequest(requestEvent); // register event
      delay(10);  
      time++;
    }
    //change to read mode
    
    read_value = 0;
    while (read_value == 0) {
      Wire.begin(10); 
      Wire.requestFrom(20, 1);    // request 1 bytes from slave device #20
      read_value = Wire.read();    // receive byte as an integer
      delay(1);
    }

    secondaryBoard[myGuess[0] - 1][myGuess[1] - 1] = read_value / 10;
    drawTracOnes();
    if (read_value % 10 == 1) {
      gameOver = true;
    }
    
    if (gameOver) {
      winner = true;
      break;
    }

    delay(100);
    read_value = 0;
    while (read_value == 0) {
      Wire.begin(10); 
      Wire.requestFrom(20, 1);    // request 1 bytes from slave device #20
      read_value = Wire.read();    // receive byte as an integer 
      delay(1);
    }
    otherGuess[0] = (read_value /  10) - 1;
    otherGuess[1] = (read_value % 10) - 1;
    
    //change to write mode
    int val = hitOrMiss();
    int x = 0;

    primaryBoard[otherGuess[0]][otherGuess[1]] = val;
    drawPrimOnes();

    checkGameOver();
    
    if (gameOver) {
      winner = false;
      x = 1;
    }
    
    //send val
    trans_value = val*10 + x;
    time = 0;
    while(time < 10) {
      Wire.begin(10); 
      Wire.onRequest(requestEvent); // register event
      delay(10);  
      time++;
    }
  }
  
  matrix2.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix2.setTextSize(1);
  matrix2.setTextColor(LED_RED);
  matrix1.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix1.setTextSize(1);
  matrix1.setTextColor(LED_GREEN);
  for (int8_t x=7; x>=-36; x--) {
    matrix1.clear();
    matrix1.setCursor(x,0);
    if (winner) {
      matrix1.print("WIN!");
    } else {
      matrix1.print("LOSE");
    }
    matrix1.writeDisplay();
    matrix2.clear();
    matrix2.setCursor(x,0);
    matrix2.print("YOU ");
    matrix2.writeDisplay();
    delay(100);
  }
  gameOver = false;
  
  primaryBoardClear();
  secondaryBoardClear();

  otherSetUp = false;
}

// This method runs in the background and catches any serial input, use it and copy paste into
// code. You could use a boolean flag to notice when it is called and then in main/loop, run
// something related to the integer value created from it

// The sent value should be two digits long. First one is xpos_speech and second one 
// is ypos_speech
void serialEvent() {

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
    drawPrimOnes(); //This draws all the ships on the board
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

void drawPrimOnes() {
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      if (primaryBoard[y][x] == 1) {
        matrix1.drawPixel(x, y, LED_RED);
        matrix1.writeDisplay();
      } else if (primaryBoard[y][x] == 2) {
        matrix1.drawPixel(x, y, LED_GREEN);
        matrix1.writeDisplay();
      } else if (primaryBoard[y][x] == 3) {
        matrix1.drawPixel(x, y, LED_YELLOW);
        matrix1.writeDisplay();
      }
    }
  }
}

void drawTracOnes() {
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      if (secondaryBoard[y][x] == 1) {
        matrix2.drawPixel(x, y, LED_YELLOW);
        matrix2.writeDisplay();
      } else if (secondaryBoard[y][x] == 2) {
        matrix2.drawPixel(x, y, LED_GREEN);
        matrix2.writeDisplay();
      } else if (secondaryBoard[y][x] == 3) {
        matrix2.drawPixel(x, y, LED_RED);
        matrix2.writeDisplay();
      }
    }
  }
}

void requestEvent() {
  Wire.write(trans_value); // respond with message of 1 byte
}

int hitOrMiss() {
  int result = primaryBoard[otherGuess[0]][otherGuess[1]];
  if (result == 0) {
    return 2;
  } else if (result == 1) {
    return 3;
  }
}

void makeGuess() {
    char in = 0;
    in = keypad.getKey();
    int pos = 0;
    while (pos < 2) {
        if(Serial.available()) {
          char ch = Serial.read();
          Serial.print(ch);
          Serial.write(ch);
          if(isDigit(ch)) {
            if(!index) {
              index = 1;
              xpos_speech = (int)ch - 48;
              Serial.print(xpos_speech);
            } else if (index) {
              index = 0;
              ypos_speech = (int)ch - 48;
              Serial.print(ypos_speech);
              eventCalled = 1;
            }
          }
        }
      if (eventCalled == 1) {
        pos = 2;
        myGuess[1] = ypos_speech;
        myGuess[0] = xpos_speech;
        eventCalled = 0;
      }
      if (in != 0) {
        int xAscii = (int) in;
        int x = xAscii - 48;
        Serial.println(x);
        if (x > 0 && x < 9) {
         myGuess[pos] = x;
         pos++; 
        }
      }
      in = keypad.getKey();
      delay(100);
      if(pos == 2 && secondaryBoard[myGuess[0] - 1][myGuess[1] - 1] != 0) {
        pos = 0;
      }
   }
}
/** OLD MAKEGUESS()
void makeGuess() {
    char in = 0;
    in = keypad.getKey();
    int pos = 0;
    while (pos < 2) {
      if (in != 0) {
        int xAscii = (int) in;
        int x = xAscii - 48;
        Serial.println(x);
        if (x > 0 && x < 9) {
         myGuess[pos] = x;
         pos++; 
        }
      }
      in = keypad.getKey();
      delay(100);
      if(pos == 2 && secondaryBoard[myGuess[0] - 1][myGuess[1] - 1] != 0) {
        pos = 0;
      }
   }
}
 */
void checkGameOver() {
  int ones = 0;
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      if (primaryBoard[y][x] == 1) {
        ones++;
      }
    }
  }
  if (ones == 0) {
    gameOver = true;
    return;
  }
  gameOver = false;
}

void primaryBoardClear() {
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      primaryBoard[i][j] = 0;
    }
  }
}


void secondaryBoardClear() {
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      secondaryBoard[i][j] = 0;
    }
  }
}



