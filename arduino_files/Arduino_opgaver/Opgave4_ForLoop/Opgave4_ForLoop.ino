#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

byte frame[8][12];


void setup() {
  matrix.begin();
}


void loop() {
  
  for (int row = 8; row > -1; row--) {
    for (int column = 0; column < 12; column++){
    frame[row][column] = 1;
    }
    
    matrix.renderBitmap(frame, 8, 12);
    delay(500);

    for (int column = 0; column < 12; column++){
      frame[row][column] = 0;
    }
  }

  for (int column = 0; column < 12; column++) {
    for (int row = 0; row < 8; row++){
    frame[row][column] = 1;
    }
    
    matrix.renderBitmap(frame, 8, 12);
    delay(500);

    for (int row = 0; row < 8; row++){
      frame[row][column] = 0;
    }
  }
}