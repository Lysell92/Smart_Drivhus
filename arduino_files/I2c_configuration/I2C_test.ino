#include <Wire.h>

void setup()
{
    Serial.begin(9600);
    Wire.begin();

    Serial.println("I2C Scanner");
}

// Sparkfun i2c test
void loop()
{
    byte error;
    int devices = 0;

    Serial.println("Scanning...");

    for (byte address = 1; address < 127; address++)
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            Serial.print("I2C device found at 0x");

            if (address < 16)
                Serial.print("0");

            Serial.println(address, HEX);

            devices++;
        }
    }

    if (devices == 0)
        Serial.println("No I2C devices found.");

    Serial.println();
    delay(2000);
}