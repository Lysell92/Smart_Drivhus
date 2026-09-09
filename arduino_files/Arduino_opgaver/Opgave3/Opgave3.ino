#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;
String inputBuffer = "   ";

void setup() {
  Serial.begin(9600);
  matrix.begin();
  matrix.textFont(Font_5x7);
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(50);
}

void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      matrix.beginDraw();
      matrix.beginText(0, 1, 0xFFFFFF);
      matrix.println(inputBuffer);
      matrix.endText(SCROLL_LEFT);
      matrix.endDraw();
      inputBuffer = "    ";
    } else if (c != '\r') {
      inputBuffer += c;
    }
  }
}
