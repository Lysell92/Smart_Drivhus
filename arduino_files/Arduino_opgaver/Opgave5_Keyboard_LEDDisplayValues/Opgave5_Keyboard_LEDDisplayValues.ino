#include <Arduino_LED_Matrix.h> 
#include <cmath> 

ArduinoLEDMatrix matrix; 

const int xAxisPin = A0; 
const int yAxisPin = A1; 

const int resolution = 12; 
const int maxValue = (1 << resolution) - 1; 
const int center = maxValue / 2; 

int xAxisState = 0; 
int yAxisState = 0; 

byte frame[8][12]; 


void setup() 
{ 
  Serial.begin(9600); 
  pinMode(xAxisPin, INPUT); 
  pinMode(yAxisPin, INPUT); 
  
  analogReadResolution(resolution); 
  
  matrix.begin(); 
}

void clearFrame() { 
  for (int y = 0; y < 8; y++) 
    { for (int x = 0; x < 12; x++) 
      { frame[y][x] = 0; 
    } 
    } 
  } 

void setPixel(int x, int y) { 
  if (x >= 0 && x < 12 && y >= 0 && y < 8) 
  { frame[y][x] = 1; } } 
  
void drawBar(int y, int value) { 
// Convert the 12-bit value to a bar between 0 and 11 LEDs. 
int length = map(value, 0, maxValue, 0, 12); 

for (int x = 0; x < length; x++) 
  { 
  setPixel(x, y); 
  }
}
    
void loop() 
{ 
  xAxisState = analogRead(xAxisPin); 
  yAxisState = analogRead(yAxisPin); 
  
  // Calculate how far the joystick is from its center position. 
  float x = xAxisState - center; 
  float y = yAxisState - center; 
  float magnitude = sqrt(x * x + y * y); 
  
  // Maximum possible distance from the center. 
  float maxMagnitude = sqrt(center * center + center * center); 
  
  // Convert magnitude to the same 0-4095 range. 
  int magnitudeValue = constrain( (magnitude / maxMagnitude) * maxValue, 0, maxValue ); clearFrame(); 
  
  // Three horizontal bars: // Top = X value // Middle = Y value // Bottom = joystick movement/magnitude 
  drawBar(1, xAxisState); 
  drawBar(3, yAxisState); 
  drawBar(5, magnitudeValue); 
  
  // Separate the three values visually. 
  setPixel(11, 1); 
  setPixel(11, 3); 
  setPixel(11, 5); 

  matrix.renderBitmap(frame, 8, 12);
  
  delay(50); 
}
