#include <HCSR04.h>

#define TRIG_PIN 9
#define ECHO_PIN 10

UltraSonicDistanceSensor distanceSensor(TRIG_PIN, ECHO_PIN);

void setup()
{
    Serial.begin(115200);
    delay(2000);
}

void loop()
{
    float distance = distanceSensor.measureDistanceCm();

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    delay(500);
}
