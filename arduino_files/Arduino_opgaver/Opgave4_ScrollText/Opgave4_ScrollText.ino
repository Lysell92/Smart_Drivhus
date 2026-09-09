#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

void setup() {
  matrix.begin();
}

void loop() {
  matrix.beginDraw();
  matrix.textScrollSpeed(200);
  const char text[] = "Hi!";
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText(NO_SCROLL); //SCROLL_LEFT makes it scroll
  matrix.endDraw();
}
