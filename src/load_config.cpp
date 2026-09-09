#include "load_config.hpp"
#include <string>

Config app_config;

bool load_config(const std::string& path)
{
	std::ifstream config_file(path);

	if (!config_file)
	{
		std::cerr
			<< "Configuration err_ could not open "
			<< path << "\n";
		return false;
	}

	try 
	{
		json json_config;
		config_file >> json_config;

		app_config.broker_host = json_config.at("broker_host").get<std::string>();
		app_config.broker_port = json_config.at("broker_port").get<int>();
		app_config.raw_topic = json_config.at("raw_topic").get<std::string>();
		app_config.processed_topic = json_config.at("processed_topic").get<std::string>();
		app_config.plant_analysis_topic = json_config.at("plant_analysis_topic").get<std::string>();
		app_config.processed_plant_analysis_topic = json_config.at("processed_plant_analysis_topic").get<std::string>();
		app_config.device_id = json_config.at("device_id").get<std::string>();
		// app_config.i2c_device = json_config.at("i2c_device").get<std::string>();
		// app_config.sensor_address = json_config.at("sensor_address").get<int>();
	}

	catch(const json::exception& e)
	{
		std::cerr 
		<< "Configuration error: "
		<< e.what() << "\n";
		return false;
	}
	return true;
}