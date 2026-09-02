# edge-gateway

## Purpose

`Beagleplay-iot` is an IoT-oriented MQTT subscriber application designed to receive, parse, and manage data from external sources.

The application uses:

- **Eclipse Mosquitto** for MQTT communication.
- **nlohmann/json** for JSON parsing and data handling.

The current implementation establishes a publish/subscribe relationship where external IoT devices or services can publish telemetry data to an MQTT broker. The receiver subscribes to the configured topic, processes incoming messages, validates JSON payloads, and sends the data through to the server as a processed topic.

The architecture is designed to be extended for future IoT applications where additional devices, sensors, or processing pipelines may be added.

---

# Build Instructions

The project source code is located at: /home/beagle/projects/beagleplay-iot



First navigate to the project directory, then remove previous build artifacts. Then compile the application and then the application can be run manually:

```bash
cd /home/beagle/projects/beagleplay-iot

make clean

make

sudo ./build/beagleplay-iot --config /etc/beagle-subscriber/config.json

```

# Configuration

The application reads its runtime configuration from: /etc/beagle-subscriber/config.json

Configuration options:

Parameter	Description
broker_host	MQTT broker hostname or IP address
broker_port	MQTT broker port
topic	MQTT topic the subscriber listens to
data_file	Location where received data is stored

The application loads these values into an internal configuration structure at startup.

# Installation Layout

The compiled executable is installed at:

/opt/beagle-subscriber/
└── beagleplay-iot

Configuration: /etc/beagle-subscriber/config.json

Systemd service: /etc/systemd/system/beagleplay-iot.service

Manual page: /usr/local/share/man/man1/beagleplay-iot.1


# Service Management

The application can run as a systemd service.

Service file: /etc/systemd/system/beagleplay-iot.service

After installing or modifying the service file, reload systemd: 
sudo systemctl daemon-reload

Then you can start the service with: 
sudo systemctl start beagleplay-iot

Stop the service:
sudo systemctl stop beagleplay-iot

Enabled service on boot:
sudo systemctl enable beagleplay-iot

Check service status:
sudo systemctl status beagleplay-iot

View logs:
journalctl -u beagleplay-iot

View the manual for general information:
man beagleplay-iot


# Message Format

The receiver expects incoming MQTT messages containing JSON formatted telemetry data.

Example MQTT message:

Topic: sensors/bp-12/raw

Payload:
{
    "temperature_c": "26",
    "humidity_pct": "27.2",
    "pressure_hpa": "1018
}

The subscriber parses the incoming payload using nlohmann/json and can access individual fields for processing or storage.