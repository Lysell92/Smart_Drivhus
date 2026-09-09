#include "on_connect.hpp"
#include "load_config.hpp"

void on_connect(
	struct mosquitto *mosq, 
	void *obj, 
	int reason_code)
{
	int rc;
	(void)obj; 
	std::cout << "on_connect : " << mosquitto_connack_string(reason_code) << "\n";

	if(reason_code != 0)
	{
		mosquitto_disconnect(mosq);
		return;
	}

	std::cout
    << "Raw topic: "
    << app_config.raw_topic
    << "\n";

	std::cout
    << "Plant topic: "
    << app_config.plant_analysis_topic
    << "\n";

	rc = mosquitto_subscribe(mosq, NULL, app_config.raw_topic.c_str(), 1);
	if(rc != MOSQ_ERR_SUCCESS){
		std::cerr 
			<< "Error subscribing: " 
			<< mosquitto_strerror(rc) 
			<< "\n";
		
		mosquitto_disconnect(mosq);
		return;
	}

    rc = mosquitto_subscribe(mosq, NULL, app_config.plant_analysis_topic.c_str(), 1);

    if (rc != MOSQ_ERR_SUCCESS)
    {
        std::cerr
            << "Error subscribing to plant analysis topic: "
            << mosquitto_strerror(rc)
            << "\n";

        mosquitto_disconnect(mosq);
        return;
    }

}