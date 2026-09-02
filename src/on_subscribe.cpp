#include "on_subscribe.hpp"

void on_subscribe(struct mosquitto *mosq, 
	void *obj, 
	int mid, 
	int qos_count, 
	const int *granted_qos)
{
	(void)mosq;
	(void)obj;
	(void)mid;
	(void)qos_count;
	
	std::cout << "Subscribed with QoS: " << granted_qos[0] << "\n";
}