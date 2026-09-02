#pragma once

#include <iostream>
#include <mosquitto.h>



void on_connect(
	struct mosquitto *mosq, 
	void *obj, 
	int reason_code
	
);