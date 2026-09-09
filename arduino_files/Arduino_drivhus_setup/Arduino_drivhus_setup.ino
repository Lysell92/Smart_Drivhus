#include <WiFiS3.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_TSL2591.h>
#include <PubSubClient.h>
#include "SparkFun_Soil_Moisture_Sensor.h"
#include <HCSR04.h>

//Wifi
#define WIFI_SSID "prog"
#define WIFI_PASS "1234567890"

// MQTT
#define MQTT_SERVER "192.168.103.11"
#define MQTT_PORT 1883
#define MQTT_TOPIC "sensors/drivhusServer/raw"

// Distance sensor(HC-SR04)
#define TRIG_PIN 9
#define ECHO_PIN 10

// Pump
const int IN1 = 2;
const int IN2 = 3;

const unsigned long pumpRunDuration = 3000;
const unsigned long pumpWateringCooldown = 14400000;

unsigned long pumpStartMillis = 0;
unsigned long lastWateringMillis = 0;

bool pumpRunning(false);
bool hasWatered(false);

unsigned int pumpActivations = 0;

// Soil

const int soilWateringThreshold = 80;

// Distance measuring

const float pumpWaterSurfaceLevelThreshold = 12.5; 

// Wifi timing
const unsigned long wifiCheckMs = 5000;
unsigned long lastWifiCheck = 0;

const unsigned long sensorIntervalMs = 10000;
unsigned long lastSensorRead = 0;
const unsigned long safetyIntervalMs = 500;
unsigned long lastSafetyCheck = 0;


// Sensors
Adafruit_BME280 bme;
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);
SparkFunSoilMoistureSensor mySoilSensor;
UltraSonicDistanceSensor distanceSensor(TRIG_PIN, ECHO_PIN);

// Sensor data

struct SensorData
{
    float temperature;
    float humidity;
    float pressure;
    float lux;
    int soilMoisture;
    float distance;
    int pumpActivations;
};

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup()
{
    Serial.begin(9600);
    
    setupWiFi();
    setupMQTT();

    setupBME();
    setupTSL();
    setupSparkfun();
    setupPump();
}

void loop()
{
    maintainWiFi();
    maintainMQTT();

   if (client.connected())
   {
        client.loop();
   }

   if (millis() - lastSafetyCheck >= safetyIntervalMs)
   {
        lastSafetyCheck = millis();
        
        pumpRunTime();

        float distance = distanceSensor.measureDistanceCm();

        if (distance >= pumpWaterSurfaceLevelThreshold)
        {
            pumpOff();
        }
   }

   

   if (millis() - lastSensorRead >= sensorIntervalMs)
   {
    lastSensorRead = millis();

    SensorData data = readSensors();

    runPumpLogic(data);

    publishData(data);
   }
}


// Humidity/temperature sensor
void setupBME()
{
    Serial.println("Initializing BME280...");

    if (!bme.begin(0x76))
    {
        Serial.println("BME280 not found!");
        while (1);
    }

    Serial.println("BME280 initialized!");
}

// Light sensor
void setupTSL()
{
    if (!tsl.begin()) {
    Serial.println("Could not find TSL2591!");
    while (1);
    }
  
  tsl.setGain(TSL2591_GAIN_MED);
  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);

  Serial.println("TSL2591 found!");
}

void setupSparkfun()
{
    Wire.begin();

    if (mySoilSensor.begin(0x2A) == false)
    {
        Serial.println("Soil Moisture Sensor not detected!");
        while (1);
    }

    Serial.println("Soil Moisture Sensor detected.");
}

SensorData readSensors()
{
    SensorData data;
    
    // BME 280
    data.temperature = bme.readTemperature();
    data.humidity = bme.readHumidity();
    data.pressure = bme.readPressure() / 100.0F;
    
    // TSL2591
    uint32_t lum = tsl.getFullLuminosity();
    uint16_t ir = lum >> 16;
    uint16_t full = lum & 0xFFFF;
    data.lux = tsl.calculateLux(full, ir);
    
    // Sparkfun
    uint16_t soilMoisture = mySoilSensor.readMoistureValue();
    data.soilMoisture = soilMoisture;

    // HC-SR04
    data.distance = distanceSensor.measureDistanceCm();

    // Number of times the pump has been active
    data.pumpActivations = pumpActivations;
    

    return data;
}

// Waterpump logic

void runPumpLogic(const SensorData& data)
{
    bool soilIsDry =
    data.soilMoisture < soilWateringThreshold;

    bool waterLevelSafe =
    data.distance >= 0 &&
    data.distance < pumpWaterSurfaceLevelThreshold;
    
    
    Serial.print("Soil moisture: ");
    Serial.print(data.soilMoisture);

    Serial.print(" | Distance: ");
    Serial.print(data.distance);

    Serial.print(" | Soil dry enough: ");
    Serial.print(data.soilMoisture > soilWateringThreshold ? "yes" : "no");

    Serial.print(" | Water level safe: ");
    Serial.println(
    data.distance > pumpWaterSurfaceLevelThreshold ? "yes" : "no"
    );


    if (soilIsDry && waterLevelSafe)
    {
        pumpOn();
    }
    else
    {
        if (!soilIsDry)
        {
            Serial.println("Pump blocked: soil is sufficiently moist.");
        }

        if (!waterLevelSafe)
        {
            Serial.println("Pump blocked: water level unsafe or invalid.");
        }

        pumpOff();
    }

}

void setupPump()
{
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    pumpOff();
}

void pumpOn()
{
    if(pumpRunning)
    {
        return;
    }

    unsigned long now = millis();


    if (hasWatered && (now - lastWateringMillis < pumpWateringCooldown))  
    {
        Serial.println("Water is still in cooldown.");
        return;
    }  


    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    pumpRunning = true;
    pumpStartMillis = now;
    lastWateringMillis = now;
    hasWatered = true;
    pumpActivations += 1;
    
    Serial.println("Pump turned on");
    
}

void pumpOff()
{
    if(!pumpRunning)
    {
        return;
    }

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    pumpRunning = false;

    Serial.println("Pump turned off");
}

void pumpRunTime()
{
    if(!pumpRunning)
    {
        return;
    }

    unsigned long now = millis();

    if((now - pumpStartMillis) >= pumpRunDuration)
    {
        pumpOff();
    }
}

void publishData(const SensorData& data)
{
    if(!client.connected())
    {
        return;
    }
    char payload[250];

    snprintf(
    payload,
    sizeof(payload),

    "{\"temperature_c\":%.2f,"
    "\"humidity_pct\":%.2f,"
    "\"pressure_hpa\":%.2f,"
    "\"lux\":%.2f,"
    "\"soil_moisture\":%d,"
    "\"distance_cm\":%.2f,"
    "\"pump_activations\":%d}",   
    data.temperature,
    data.humidity,
    data.pressure,
    data.lux,
    data.soilMoisture,
    data.distance,
    data.pumpActivations
    );

    if (client.publish(MQTT_TOPIC, payload))
    {
        Serial.print("Published: ");
        Serial.println(payload);
    }
    else
    {
        Serial.println("MQTT publish failed!");
    }

}

void setupMQTT()
{
    client.setServer(MQTT_SERVER, MQTT_PORT);
}

void maintainMQTT()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }

    if (client.connected())
    {
        return;
    }
    reconnectMQTT();
}

// ConnectivityMQTT
void reconnectMQTT()
{
    
    Serial.println("Connecting to MQTT...");

    if (client.connect("arduino-drivhusServer"))
    {
        Serial.println("MQTT connected!");
    }
    else
    {
        Serial.print("MQTT connection failed, state=");
        Serial.println(client.state());
    }
}



// Connectivity WiFi
void setupWiFi()
{
    while (!Serial);

    Serial.println("Connecting to WiFi...");

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected!");

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

void maintainWiFi()
{
    if (millis() - lastWifiCheck < wifiCheckMs) 
    {
        return;
    }
    lastWifiCheck = millis();

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi disconnected.: Reconnecting...");
        
        WiFi.disconnect();
        delay(100);
        
        WiFi.begin(WIFI_SSID, WIFI_PASS);
    }
}