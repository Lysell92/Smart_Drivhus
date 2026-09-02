#pragma once

#include <iostream>
#include <mosquitto.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

class MessageHandler
{
    public:
    static void on_message( 
        struct mosquitto *mosq, 
        void *obj, 
        const struct mosquitto_message *msg
    );


    static bool validate_message(
        const json& message,
        std::string& error_message
    );
};