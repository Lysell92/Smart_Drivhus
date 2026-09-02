#pragma once

#include <iostream>

void on_subscribe(struct mosquitto *mosq, 
	void *obj, 
	int mid, 
	int qos_count, 
	const int *granted_qos
);