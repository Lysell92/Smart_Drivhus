#include <iostream>
#include <mosquitto.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <chrono>

#include "MessageHandler.hpp"
#include "load_config.hpp"
#include "on_subscribe.hpp"
#include "on_connect.hpp"


void print_help()
{
	std::cout 
	<< "usage:\n"
	<< " beagleplay-iot --help\n"
	<< " beagleplay-iot --config <path>\n\n"
	<< "Options:\n"
	<< " --help				Show this help text and exit\n"
	<< " --config <path>	Read configuration from specified file\n"; 
}

int main(int argc, char* argv[])
{
	if(argc == 2 && std::string(argv[1]) == "--help")
	{
		print_help();
		return 0;
	}

	if (argc != 3 || std::string(argv[1]) != "--config")
	{
		std::cerr << "Usage: beagleplay-iot -- config <path>\n";
		return 2;
	}

	const std::string config_path =argv[2];

	if (!load_config(config_path))
	{
		return 2;
	}

	

    mosquitto_lib_init();

    struct mosquitto* mosq =
        mosquitto_new(NULL, true, NULL);

    if (mosq == nullptr)
    {
        std::cerr << "Error: Out of memory.\n";
        mosquitto_lib_cleanup();
        return 1;
    }


	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_message_callback_set(mosq, MessageHandler::on_message);
	mosquitto_subscribe_callback_set(mosq, on_subscribe);

	
	int rc = mosquitto_connect(mosq, app_config.broker_host.c_str(), app_config.broker_port, 60);

	std::cout << "connect rc = "
          << rc
          << " ("
          << mosquitto_strerror(rc)
          << ")\n";

	if (rc != MOSQ_ERR_SUCCESS)
	{
		mosquitto_destroy(mosq);
		std::cerr << "Error: " << mosquitto_strerror(rc) << "\n";
		return 1;
	}

	std::cout << "Starting MQTT loop\n";

	mosquitto_loop_start(mosq);

	 while (true)
	{
		std::this_thread::sleep_for(
			std::chrono::seconds(10)
		);
	}

	mosquitto_loop_stop(mosq, true);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	return 0;
}



