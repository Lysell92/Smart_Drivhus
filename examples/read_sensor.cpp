#include "bme280_sensor.hpp"

#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr
            << "Usage: " << argv[0]
            << " <i2c-device> <address>\n\n"
            << "Examples:\n"
            << "  " << argv[0] << " /dev/play/grove/i2c 0x76\n"
            << "  " << argv[0] << " /dev/play/qwiic/i2c 0x77\n";
        return 2;
    }

    try {
        const std::string device = argv[1];
        const auto parsed_address = std::stoul(argv[2], nullptr, 0);

        if (parsed_address > 0x7f) {
            throw std::runtime_error("I2C address must be a 7-bit address");
        }

        Bme280Sensor sensor(
            device,
            static_cast<std::uint8_t>(parsed_address));

        sensor.init();

        const auto value = sensor.read();

        std::cout << std::fixed << std::setprecision(2)
                  << "temperature_c=" << value.temperature_c << '\n'
                  << "humidity_pct=" << value.humidity_pct << '\n'
                  << "pressure_hpa=" << value.pressure_hpa << '\n';
    }
    catch (const std::exception& error) {
        std::cerr << "BME280 error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
