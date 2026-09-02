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
	}

	rc = mosquitto_subscribe(mosq, NULL, app_config.raw_topic.c_str(), 1);
	if(rc != MOSQ_ERR_SUCCESS){
		std::cerr << "Error subscribing: " << mosquitto_strerror(rc) << "\n";
		mosquitto_disconnect(mosq);
	}
}