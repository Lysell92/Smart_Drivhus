#include "MessageHandler.hpp"
#include "load_config.hpp"

void MessageHandler::on_message(
    struct mosquitto *mosq, 
    void *obj, 
    const struct mosquitto_message *msg
)
{
    (void)obj;
    (void)mosq;
    std::string payload 
    (
        static_cast<char *>(msg->payload), 
        msg->payloadlen
    );
        
    std::cout 
    << msg->topic <<" " 
    << msg->qos << " "
    << payload
    << '\n';

    try 
    {
        json telemetry = json::parse(payload); 
        
        std::string error_message;

        if (!validate_message(telemetry, error_message))
        {
            std::cerr
                << "Invalid telemetry: "
                << error_message
                << '\n';

            return;
        }



        auto now = std::chrono::system_clock::now();
        std::time_t time_now = std::chrono::system_clock::to_time_t(now);
        std::tm utc_time = *std::gmtime(&time_now);
        std::ostringstream timestamp_stream;

        timestamp_stream 
            << std::put_time(&utc_time, "%Y-%m-%dT%H:%M:%SZ");
        std::string timestamp = timestamp_stream.str();
        
        telemetry["device_id"] = app_config.device_id;
        telemetry["timestamp"] = timestamp;

        std::string processed_payload = telemetry.dump();

        int rc = mosquitto_publish
        (
            mosq,
            nullptr,
            app_config.processed_topic.c_str(),
            static_cast<int>(processed_payload.size()),
            processed_payload.c_str(),
            1,
            false
        );

        if (rc != MOSQ_ERR_SUCCESS)
        {
            std::cerr
                << "Error publishing processed telemetry: "
                << mosquitto_strerror(rc)
                << '\n';
        }
    }
    catch(const json::exception& e)
    {
        std::cerr   
            << "JSON processing error: " 
            << e.what() 
            << "\n";
    }
}

bool MessageHandler::validate_message(
    const json& message, 
    std::string& error_message
)
{
    if (!message.is_object())
    {
        error_message = "Message must be a JSON object";
        return false;
    }

    if (!message.contains("humidity_pct") || !message["humidity_pct"].is_number())
    {
        error_message = "Humidity should be a number";
        return false;
    }

    if (!message.contains("temperature_c") || !message["temperature_c"].is_number())
    {
        error_message = "Temperature should be a number";
        return false;
    }

    if (!message.contains("pressure_hpa") || !message["pressure_hpa"].is_number())
    {
        error_message = "Pressure should be a number";
        return false;
    }

    if(message["temperature_c"] < 0 || message["temperature_c"] > 100)
    {
        error_message = "Inside temperatures normally doesn't to these temperatures, something is wrong brother";
        return false;
    }

    return true;
}
