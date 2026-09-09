#include <WiFiS3.h>
#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup()
{
    Serial.begin(9600);
    while (!Serial);

    Serial.println("Connecting to WiFi...");

    WiFi.begin("prog", "1234567890");

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

    // MQTT broker = BeaglePlay
    client.setServer("172.31.0.75", 1883);

    Serial.println("Connecting to MQTT...");

    if (client.connect("arduino-bp12"))
    {
        Serial.println("MQTT connected!");

        bool success = client.publish(
            "sensors/bp-12/raw",
            "{\"test\":true}"
        );

        if (success)
        {
            Serial.println("Test message published!");
        }
        else
        {
            Serial.println("Publish failed!");
        }
    }
    else
    {
        Serial.print("MQTT connection failed, state=");
        Serial.println(client.state());
    }
}

void loop()
{
    if (!client.connected())
    {
        Serial.println("MQTT disconnected!");
    }

    client.loop();
}
