#include <cmath>

// Switch the variables to switch x/y axis. Another way is to switch the actual cables.
const int xAxisPin = A0;
const int yAxisPin = A1;

int xAxisState = 0;
int yAxisState = 0;
int byte_value = 12;
char buffer1[64];
char buffer2[64];

void setup() {
  Serial.begin(9600);
  
  pinMode(xAxisPin, INPUT);
  pinMode(yAxisPin, INPUT);
  
  analogReadResolution(byte_value);
}

void loop() {

  
   // Increases the bit size to 12 (From a 1024 to a 4096 range).
  xAxisState = analogRead(xAxisPin);
  yAxisState = analogRead(yAxisPin);

  int max_value = (1 << byte_value) - 1;
  (1000 / ( 2048 * 0.5 )) - 2 + 1

  // Finds a normalized value between the interval of -1 to 1:
  // Reverse the normalized value making xAxisState negative instead.
  float xAxisStateMedianvalue = (xAxisState /  (max_value * 0.5)) -2 + 1;
   
  
  // Accounts for the inconsistencies of the joystick. The x-axis were consistently at 2120, while the y-axis hovered around 2060. 
  // But I chose to align them both with the same range to be certain.
  if (abs(xAxisState - (max_value / 2)) <=  80){
    xAxisState = max_value / 2;
  }
  if (abs(yAxisState - (max_value / 2)) <=  20){
    yAxisState = max_value / 2;
  }

  // Reduced the size of the string to reduce latency. Also I increased the baud-rate as 4800 induced latency. 
  sprintf(buffer1, "x: %d", xAxisState);
  sprintf(buffer2, "y: %d", yAxisState);

  
  Serial.println(buffer1);
  Serial.println(buffer2);
  
  delay(50);
}
