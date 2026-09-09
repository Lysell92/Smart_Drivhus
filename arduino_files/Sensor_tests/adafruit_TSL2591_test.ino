#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2591.h>

Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);

void setup() {
  Serial.begin(9600);

  if (!tsl.begin()) {
    Serial.println("Could not find TSL2591!");
    while (1);
  }

  Serial.println("TSL2591 found!");

  // Set the gain
  tsl.setGain(TSL2591_GAIN_MED);

  // Set integration time
  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);
}

void loop() {
  uint32_t lum = tsl.getFullLuminosity();

  uint16_t ir = lum >> 16;
  uint16_t full = lum & 0xFFFF;

  uint16_t visible = full - ir;

  Serial.print("Visible: ");
  Serial.print(visible);

  Serial.print(" | IR: ");
  Serial.print(ir);

  Serial.print(" | Full spectrum: ");
  Serial.println(full);

  Serial.print("Lux: ");
  Serial.println(tsl.calculateLux(full, ir));

  Serial.println("--------------------");

  delay(1000);
}
