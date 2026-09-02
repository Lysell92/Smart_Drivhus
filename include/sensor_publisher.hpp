#pragma once

#include <mosquitto.h>
#include <string>
#include"bme280_sensor.hpp"

class SensorPublisher
{
public:
    SensorPublisher(
        mosquitto* mosq,
        const std::string& i2c_device,
        int sensor_address,
        const std::string& raw_topic
    );

    bool init();
    bool publish();

private:
    mosquitto* mosq_;
    std::string raw_topic_;
    std::string i2c_device_;
    Bme280Sensor sensor_;
};