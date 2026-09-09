#include "SparkFun_Soil_Moisture_Sensor.h"
#include <Wire.h>

SparkFunSoilMoistureSensor mySoilSensor;

#define CURRENT_ADDRESS 0x29
#define NEW_ADDRESS     0x2A

void setup()
{
    Serial.begin(115200);
    delay(2000);

    Wire.begin();

    Serial.println("Looking for soil sensor at 0x29...");

    if (!mySoilSensor.begin(CURRENT_ADDRESS))
    {
        Serial.println("Soil sensor NOT found at 0x29!");
        return;
    }

    Serial.println("Soil sensor found!");
    Serial.println("Changing address from 0x29 to 0x2A...");

    if (mySoilSensor.setI2CAddress(NEW_ADDRESS) != ksfTkErrOk)
    {
        Serial.println("Failed to change address!");
        return;
    }

    Serial.println("Address changed to 0x2A!");
}

void loop()
{
}
