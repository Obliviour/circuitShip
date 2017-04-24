#include <Wire.h>;
#include <Key.h>;
#include <Keypad.h>;

/**
 * Arduino2 starts as a READER, press key 9 to change to WRITER
 * Change writer->reader before changing reader->writer
 */

int trans_value; // value to be transmitted
boolean write_read = false;  // true if write, false if read
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
void setup() {
  Serial.begin(9600);  // start serial for output
  trans_value = 0;
  /**INITALIZED **/
  Wire.begin(20);                // join i2c bus with address #20
}

void loop() {

  char in = keypad.getKey();

  if(in != '9') {
    write_read = !write_read;
  }
  if(write_read) {
    /** WRITE TRANSMISSION**/
    Wire.onRequest(requestEvent); // register event
    delay(100);
  } else {
    /** READ DATA **/
    Wire.requestFrom(10, 1);    // request 6 bytes from slave device #10
    int x = Wire.read();    // receive byte as an integer
    Serial.println(x);         // print the integer
    if (trans_value != x+1) {
      trans_value = x+1;  
    }
    delay(100);
  }
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  Wire.write("%d",trans_value); // respond with message of 6 bytes
}
