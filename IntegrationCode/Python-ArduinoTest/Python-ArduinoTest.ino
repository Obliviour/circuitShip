const int len = 2;
char sentValue[len]; 
int index = 0;  
int xpos_speech=0;
int ypos_speech=0;
int eventCalled = 0;  
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);  
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);

  digitalWrite(2, LOW); 
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);   
  // setup serial channel
  Serial.begin(9600);
  
}
void loop() {
  /**
  if (xpos_speech == 1) {
    digitalWrite(LED_BUILTIN, HIGH); 
  } else if (!xpos_speech) {
    digitalWrite(LED_BUILTIN, LOW); 
  */
  if (eventCalled == 1) {
    eventCalled = 0;
    int val = digitalRead(xpos_speech);
    Serial.print(val);
    digitalWrite(xpos_speech, !val);
  }
  /**
  if (eventCalled == 1) {
    eventCalled = 0;
    setPins(ypos_speech);
    setPins(xpos_speech);
  }
}

void setPins(int val) {
  int r = 0;
  int c = 0;
  switch (val) {
    case 1:
    r=2;
    c=6;
    digitalWrite(r, LOW);
    digitalWrite(c, LOW);
    delay(1000);
    break;
    case 2:
    r=2;
    c=7;
    digitalWrite(r, LOW);
    digitalWrite(c, LOW);
    delay(1000);
    break;
    case 3:
    r=2;
    c=8;
    digitalWrite(r, LOW);
    digitalWrite(c, LOW);
    delay(1000);
    break;
    case 4:
    r=3;
    c=6;
    digitalWrite(r, LOW);
    digitalWrite(c, LOW);
    delay(1000);
    break;
    case 5:
    r=3;
    c=7;
    digitalWrite(r, LOW);
    digitalWrite(c, LOW);
    delay(1000);
    break;
    case 6:
    r=3;
    c=8;
    digitalWrite(r, LOW);
    digitalWrite(c, LOW);
    delay(1000);
    break;
    case 7:
    r=4;
    c=6;
    digitalWrite(r, LOW);
    digitalWrite(c, LOW);
    delay(1000);
    break;
    case 8:
    r=4;
    c=7;
    digitalWrite(r, LOW);
    digitalWrite(c, LOW);
    delay(1000);
    break;
    case 9:
    r=5;
    c=6;
    digitalWrite(r, LOW);
    digitalWrite(c, LOW);
    delay(1000);
    break;
  }
  digitalWrite(2, HIGH); 
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);  
  */
}

// This method runs in the background and catches any serial input, use it and copy paste into
// code. You could use a boolean flag to notice when it is called and then in main/loop, run
// something related to the integer value created from it

// The sent value should be two digits long. First one is xpos_speech and second one 
// is ypos_speech
void serialEvent() {
  while(Serial.available()) {
    char ch = Serial.read();
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
}

