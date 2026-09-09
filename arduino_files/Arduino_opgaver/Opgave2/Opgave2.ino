#include <Arduino.h>

void setup() {
    Serial.begin(9600);
}

void loop() {
    Serial.println("Hello, World from Arduino!");
    delay(1000);
}
