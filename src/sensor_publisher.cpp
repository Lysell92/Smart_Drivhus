#include "sensor_publisher.hpp"

#include <iostream>
#include <nlohmann/json.hpp>

SensorPublisher::SensorPublisher(
    mosquitto* mosq,
    const std::string& i2c_device,
    int sensor_address,
    const std::string& raw_topic
)
    : mosq_(mosq),
      raw_topic_(raw_topic),
      sensor_(i2c_device, sensor_address)
{
}

bool SensorPublisher::init()
{
    sensor_.init();
    return true;
}

bool SensorPublisher::publish()
{
    std::cout << "SensorPublisher::publish() called\n";

    auto measurement = sensor_.read();

    nlohmann::json telemetry =
    {
        {"temperature_c", measurement.temperature_c},
        {"humidity_pct", measurement.humidity_pct},
        {"pressure_hpa", measurement.pressure_hpa}
    };

    std::cout << "Publishing raw telemetry to: "
              << raw_topic_ << '\n';

    std::string payload = telemetry.dump();

    int rc = mosquitto_publish(
        mosq_,
        nullptr,
        raw_topic_.c_str(),
        static_cast<int>(payload.size()),
        payload.c_str(),
        1,
        false
    );

    std::cout << "mosquitto_publish rc = "
            << rc << " ("
            << mosquitto_strerror(rc)
            << ")\n";

    if (rc != MOSQ_ERR_SUCCESS)
    {
        std::cerr
            << "Publish error: "
            << mosquitto_strerror(rc)
            << '\n';

        return false;
    }

    return true;
}