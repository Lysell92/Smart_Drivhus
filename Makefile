CXX ?= g++
CC  ?= gcc

CPPFLAGS := -Iinclude -Ivendor/bosch

CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O2
CFLAGS   := -std=c11 -Wall -Wextra -Wpedantic -O2

LDLIBS := -lmosquitto

BUILD := build

CPP_OBJECTS := \
    $(BUILD)/main.o \
    $(BUILD)/MessageHandler.o \
    $(BUILD)/load_config.o \
    $(BUILD)/on_connect.o \
    $(BUILD)/on_subscribe.o \
	## $(BUILD)/linux_i2c.o \
    $(BUILD)/sensor_publisher.o \
	$(BUILD)/bme280_sensor.o

## BOSCH_OBJECT := $(BUILD)/bme280.o

.PHONY: all clean check-vendor

all: $(BUILD)/drivhus-iot

check-vendor:
	@test -f vendor/bosch/bme280.c || \
		(echo "Bosch files missing. Run: ./scripts/fetch_bosch.sh" && exit 1)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/main.o: src/main.cpp | $(BUILD) check-vendor
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD)/MessageHandler.o: src/MessageHandler.cpp include/MessageHandler.hpp | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

## $(BUILD)/bme280_sensor.o: src/bme280_sensor.cpp include/bme280_sensor.hpp include/linux_i2c.hpp | $(BUILD) check-vendor \
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

## $(BUILD)/linux_i2c.o: src/linux_i2c.cpp include/linux_i2c.hpp | $(BUILD) \
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD)/load_config.o: src/load_config.cpp include/load_config.hpp | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD)/on_connect.o: src/on_connect.cpp include/on_connect.hpp include/load_config.hpp | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD)/on_subscribe.o: src/on_subscribe.cpp include/on_subscribe.hpp | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

## $(BUILD)/sensor_publisher.o: src/sensor_publisher.cpp include/sensor_publisher.hpp include/bme280_sensor.hpp | $(BUILD) check-vendor \
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

## $(BOSCH_OBJECT): vendor/bosch/bme280.c vendor/bosch/bme280.h vendor/bosch/bme280_defs.h | $(BUILD) \
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/drivhus-iot: $(CPP_OBJECTS) ##$(BOSCH_OBJECT)
	$(CXX) $^ $(LDLIBS) -o $@

clean:
	rm -rf $(BUILD)
