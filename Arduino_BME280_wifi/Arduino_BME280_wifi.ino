#include <WiFiS3.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <PubSubClient.h>

#define WIFI_SSID "prog"
#define WIFI_PASS "1234567890"

#define MQTT_SERVER "172.31.0.75"
#define MQTT_PORT 1883

#define MQTT_TOPIC "sensors/bp-12/raw"

Adafruit_BME280 bme;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup()
{
    Serial.begin(9600);
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

    Serial.print("Arduino IP: ");
    Serial.println(WiFi.localIP());

    Serial.print("Gateway: ");
    Serial.println(WiFi.gatewayIP());

    Serial.print("Subnet: ");
    Serial.println(WiFi.subnetMask());

    client.setServer(MQTT_SERVER, MQTT_PORT);

    Serial.println("Connecting to MQTT...");

    if (client.connect("arduino-bp12"))
    {
        Serial.println("MQTT connected!");
    }
    else
    {
        Serial.print("MQTT connection failed, state=");
        Serial.println(client.state());
    }

    Serial.println("Initializing BME280...");

    if (!bme.begin(0x76))
    {
        Serial.println("BME280 not found!");
        while (true);
    }

    Serial.println("BME280 initialized!");
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }

    client.loop();

    float temperature = bme.readTemperature();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure() / 100.0F;

    char payload[200];

    snprintf(
        payload,
        sizeof(payload),
        "{\"temperature_c\":%.2f,\"humidity_pct\":%.2f,\"pressure_hpa\":%.2f}",
        temperature,
        humidity,
        pressure
    );

    Serial.print("Publishing: ");
    Serial.println(payload);

    bool success = client.publish(
        MQTT_TOPIC,
        payload
    );

    if (success)
    {
        Serial.println("Published successfully!");
    }
    else
    {
        Serial.println("Publish failed!");
    }

    delay(10000);
}

void reconnect()
{
    while (!client.connected())
    {
        Serial.println("Connecting to MQTT...");

        if (client.connect("arduino-bp12"))
        {
            Serial.println("MQTT connected!");
        }
        else
        {
            Serial.print("MQTT connection failed, state=");
            Serial.println(client.state());

            delay(2000);
        }
    }
}



