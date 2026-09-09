# Smart Drivhus IoT

IoT software for the Smart Drivhus greenhouse project.

The system receives sensor telemetry and plant-analysis data over MQTT, processes the messages, and republishes the processed data for storage and visualization.

## Architecture

The current data flow is:

```text
Sensors
   │
   ▼
MQTT
   │
   ▼
drivhus-iot
   │
   ├── Telemetry processing
   │
   └── PlantCV analysis
          ▲
          │
       Camera
```

Processed data is published back to MQTT and consumed by Telegraf:

```text
MQTT
 │
 ├── drivhusData/drivhusServer/telemetry
 │
 └── drivhusData/drivhusServer/plant_analysis
          │
          ▼
       Telegraf
          │
          ▼
      PostgreSQL
          │
          ▼
       Grafana
```

---

# MQTT Topics

The application currently handles two types of messages.

## Sensor telemetry

Raw sensor data is received on:

```text
sensors/drivhusServer/raw
```

After processing, the data is published to:

```text
drivhusData/drivhusServer/telemetry
```

The application adds information such as the device ID and timestamp before publishing the processed message.

## Plant analysis

PlantCV publishes plant-analysis results to:

```text
plants/drivhusServer/plant_analysis
```

The C++ application receives this message and republishes it to:

```text
drivhusData/drivhusServer/plant_analysis
```

Telegraf subscribes to the processed topic.

---

# Configuration

The application loads its configuration from a JSON file.

Example:

```json
{
    "broker_host": "localhost",
    "broker_port": 1883,
    "raw_topic": "sensors/drivhusServer/raw",
    "processed_topic": "drivhusData/drivhusServer/telemetry",
    "plant_analysis_topic": "plants/drivhusServer/plant_analysis",
    "processed_plant_analysis_topic": "drivhusData/drivhusServer/plant_analysis",
    "device_id": "drivhusServer"
}
```

## Configuration options

| Parameter                        | Description                                    |
| -------------------------------- | ---------------------------------------------- |
| `broker_host`                    | MQTT broker hostname or IP address             |
| `broker_port`                    | MQTT broker port                               |
| `raw_topic`                      | MQTT topic containing raw sensor telemetry     |
| `processed_topic`                | MQTT topic for processed sensor telemetry      |
| `plant_analysis_topic`           | MQTT topic containing PlantCV analysis results |
| `processed_plant_analysis_topic` | MQTT topic for processed PlantCV results       |
| `device_id`                      | Identifier of the greenhouse/device            |

The application loads these values into an internal configuration structure at startup.

---

# Message Format

The receiver expects incoming MQTT messages containing JSON-formatted data.

## Sensor telemetry

Example:

```json
{
    "temperature_c": 26,
    "humidity_pct": 27.2,
    "pressure_hpa": 1018,
    "lux": 450,
    "soil_moisture": 42,
    "distance_cm": 15,
    "pump_activations": 2
}
```

The processed message contains the original telemetry together with information added by the subscriber, such as:

```json
{
    "temperature_c": 26,
    "humidity_pct": 27.2,
    "pressure_hpa": 1018,
    "lux": 450,
    "soil_moisture": 42,
    "distance_cm": 15,
    "pump_activations": 2,
    "device_id": "drivhusServer",
    "timestamp": "2026-09-09T15:57:09Z"
}
```

## Plant analysis

PlantCV produces JSON results similar to:

```json
{
    "plant_id": "plant_1",
    "plant_area": 539107,
    "green_percentage": 33.43,
    "image": "/home/drivhus-user2/camera-images/plant_2026-09-08_10-53-01.jpg"
}
```

The C++ subscriber adds a timestamp before publishing the processed result.

Example processed message:

```json
{
    "plant_id": "plant_1",
    "plant_area": 539107,
    "green_percentage": 33.43,
    "image": "/home/drivhus-user2/camera-images/plant_2026-09-08_10-53-01.jpg",
    "timestamp": "2026-09-09T15:56:36Z"
}
```

---

# PlantCV

Plant images are captured by the greenhouse camera and stored in:

```text
/home/drivhus-user2/camera-images/
```

A file watcher monitors this directory for new JPEG images.

When a new image is detected, the watcher runs the PlantCV pipeline.

The current repository layout is:

```text
plantCV/
└── plantcv_pipeline.py

scripts/
├── fetch_bosch.sh
├── run_plant_analysis.sh
└── watch_plant_images.sh
```

The PlantCV pipeline:

1. Reads the camera image.
2. Rotates the image 180 degrees to correct the camera orientation.
3. Crops the relevant greenhouse area.
4. Converts the image from BGR to HSV.
5. Detects green pixels.
6. Removes small isolated regions.
7. Calculates the green plant area.
8. Calculates the percentage of the analyzed area containing green pixels.
9. Creates diagnostic images.
10. Publishes the measurements to MQTT when requested.

Example result:

```json
{
    "plant_id": "plant_1",
    "plant_area": 539107,
    "green_percentage": 33.43,
    "image": "/home/drivhus-user2/camera-images/plant_2026-09-08_10-53-01.jpg"
}
```

## Running PlantCV manually

From the PlantCV environment:

```bash
python plantcv_pipeline.py ~/camera-images/plant_2026-09-08_10-53-01.jpg \
    --output-dir output \
    --mqtt
```

The Python virtual environment is kept outside the Git repository.

Generated diagnostic images are also kept outside the repository.

---

# Automatic PlantCV Processing

The image watcher is:

```text
scripts/watch_plant_images.sh
```

It monitors:

```text
/home/drivhus-user2/camera-images
```

and starts:

```text
scripts/run_plant_analysis.sh
```

for new `.jpg` and `.jpeg` files.

The watcher waits for the file's `close_write` event so that an image is processed after the camera has finished writing it.

---

# Installation Layout

The compiled C++ executable is installed at:

```text
/opt/drivhus-subscriber/
└── drivhus-iot
```

Configuration:

```text
/etc/drivhus-subscriber/config.json
```

Systemd service:

```text
/etc/systemd/system/drivhus-subscriber.service
```

The exact service name should match the installed systemd unit on the target machine.

---

# Service Management

The C++ MQTT subscriber runs as a systemd service.

After installing or modifying the service file, reload systemd:

```bash
sudo systemctl daemon-reload
```

Enable the service at boot:

```bash
sudo systemctl enable drivhus-iot
```

Start the service:

```bash
sudo systemctl start drivhus-iot
```

Stop the service:

```bash
sudo systemctl stop drivhus-iot
```

Check its status:

```bash
sudo systemctl status drivhus-iot
```

View its logs:

```bash
journalctl -u drivhus-iot
```

Follow the logs live:

```bash
journalctl -u drivhus-iot -f
```

---

# PlantCV Systemd Service

The automatic image watcher can also run as a systemd service.

Service file:

```text
/etc/systemd/system/plant-analysis.service
```

After installing or modifying the service:

```bash
sudo systemctl daemon-reload
```

Enable it at boot:

```bash
sudo systemctl enable plant-analysis.service
```

Start it:

```bash
sudo systemctl start plant-analysis.service
```

Check its status:

```bash
sudo systemctl status plant-analysis.service
```

View its logs:

```bash
journalctl -u plant-analysis.service -f
```

The service runs the image watcher as the `drivhus-user2` user.

---

# Data Storage

Telegraf consumes the processed MQTT topics and writes the data to PostgreSQL.

The current database contains separate tables for the two data types.

## Sensor telemetry

```text
drivhus
```

The telemetry table contains fields such as:

```text
time
device_id
mqtt_topic
distance_cm
humidity_pct
lux
pressure_hpa
pump_activations
soil_moisture
temperature_c
```

## Plant analysis

```text
plant_analysis
```

The plant-analysis table contains:

```text
time
mqtt_topic
plant_id
green_percentage
plant_area
image
```

This separation allows sensor telemetry and plant-analysis data to be queried independently.

---


The `plant_id` field can be used to filter measurements for a specific plant.

For example:

```sql
SELECT
    time,
    green_percentage
FROM plant_analysis
WHERE $__timeFilter(time)
  AND plant_id = 'plant_1'
ORDER BY time;
```

---

# Container Stack

The supporting infrastructure runs using Podman.

The current stack contains:

```text
iot-mosquitto
iot-postgres
iot-grafana
iot-telegraf
```

The services provide:

* **Mosquitto** — MQTT broker
* **PostgreSQL** — database
* **Telegraf** — MQTT-to-PostgreSQL data collection
* **Grafana** — data visualization

MQTT is exposed on host port:

```text
1883
```

Grafana is exposed on host port:

```text
3000
```

The containers communicate through the Podman network:

```text
17683_serverstack
```

---

# Development Workflow

The project uses Git branches to separate development from the deployed version.

The current workflow is:

```text
drivhus-user
      │
      ▼
   origin/dev
      │
      ▼
drivhus-user2
      │
      │ development/testing
      ▼
   origin/dev
      │
      ▼
/srv/drivhus-project
      │
      │ tested code promoted to master
      ▼
    master
      │
      ▼
 origin/master
```

The `dev` branch is used for ongoing development and testing.

The `master` branch represents the version promoted for deployment.

---

# Repository Structure

The important parts of the repository are currently:

```text
drivhus-iot/
├── Arduino_BME280_wifi/
├── build/
├── docs/
├── examples/
├── include/
├── plantCV/
│   └── plantcv_pipeline.py
├── scripts/
│   ├── fetch_bosch.sh
│   ├── run_plant_analysis.sh
│   └── watch_plant_images.sh
├── src/
├── vendor/
├── config.json
├── Makefile
├── README.MD
└── .gitignore
```

The Python virtual environment, camera images, and generated PlantCV output are intentionally kept outside the repository.

---

# Current Status

The complete plant-analysis data pipeline has been tested successfully:

```text
Camera
  ↓
camera-images/
  ↓
watch_plant_images.sh
  ↓
PlantCV
  ↓
plants/drivhusServer/plant_analysis
  ↓
C++ MQTT subscriber
  ↓
drivhusData/drivhusServer/plant_analysis
  ↓
Telegraf
  ↓
PostgreSQL
  ↓
plant_analysis table
  ↓
Grafana
```

Sensor telemetry follows:

```text
sensors/drivhusServer/raw
  ↓
C++ MQTT subscriber
  ↓
drivhusData/drivhusServer/telemetry
  ↓
Telegraf
  ↓
PostgreSQL
  ↓
drivhus table
  ↓
Grafana
```

Both MQTT consumers have been verified to connect successfully to the Mosquitto broker, and plant-analysis measurements have been successfully stored in PostgreSQL.
