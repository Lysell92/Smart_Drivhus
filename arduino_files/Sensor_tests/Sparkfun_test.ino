/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2025, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 * This example demonstrates how to read the soil moisture sensor value.
 * 
 * Supported Sensor:
 *  Qwiic Soil Moisture Sensor https://www.sparkfun.com/sparkfun-qwiic-soil-moisture-sensor.html
 * 
 * Hardware Connections:
 * - Connect the Qwiic Soil Moisture Sensor to the Qwiic connector of your development board (SparkFun Thing Plus, ReadBoard ...etc.)
 *   If you don't have a development board with a Qwiic connector, you can purchase one at sparkfun.com
 * - Connect the development board to your computer using a USB cable.
 * - Open the Serial Monitor at a baud rate of 115200 to see the sketch output.
 * 
 *---------------------------------------------------------------------------------
 */

#include "SparkFun_Soil_Moisture_Sensor.h" // CTRL+Click here to get the library: http://librarymanager/All#SparkFun_Soil_Moisture_Sensor
#include <Wire.h>

SparkFunSoilMoistureSensor mySoilSensor; // Create an instance of the sensor class

// Some Dev boards have their QWIIC connector on Wire or Wire1
// This #ifdef will help this sketch work across more products

#ifdef ARDUINO_SPARKFUN_THINGPLUS_RP2040
#define wirePort Wire1
#else
#define wirePort Wire
#endif


void setup()
{
    Serial.begin(115200);

    while (!Serial)
        delay(10); // Wait for Serial to become available.
    // Necessary for boards with native USB (like the SAMD51 Thing+).
    // For a final version of a project that does not need serial debug (or a USB cable plugged in),
    // Comment out this while loop, or it will prevent the remaining code from running.

    Serial.println();
    Serial.println("------------------------------------------------------------");
    Serial.println("SparkFun Soil Sensor Example 1");
    Serial.println("Basic Readings");
    Serial.println("------------------------------------------------------------");
    Serial.println();

    wirePort.begin();

    if (mySoilSensor.begin(0X2A) == false)
    {
        Serial.println("Soil Moisture Sensor not detected at default I2C address. Verify the sensor is connected. Stopping.");
        while (1)
            ;
    }
    Serial.println("Soil Moisture Sensor found!");

    Serial.println();
    Serial.println("Reading soil moisture sensor...");
    Serial.println();
}

//----------------------------------------------------------------------------------------
void loop()
{
    // Let's get some readings
    uint16_t soilMoisture = mySoilSensor.readMoistureValue();
    Serial.print("Soil Moisture: ");
    Serial.println(soilMoisture);

    // loop delay - 2 seconds
    delay(2000);
}