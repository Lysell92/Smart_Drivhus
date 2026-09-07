#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Config
{
    std::string broker_host ="localhost";
    int broker_port = 1883;
    
    std::string raw_topic;
    std::string processed_topic;
    
    std::string device_id;
};

extern Config app_config;

bool load_config(const std::string& path
);