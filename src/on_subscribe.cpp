#include "on_subscribe.hpp"

void on_subscribe(
    struct mosquitto *mosq,
    void *obj,
    int mid,
    int qos_count,
    const int *granted_qos)
{
    (void)mosq;
    (void)obj;

    std::cout
        << "SUBSCRIBE ACK"
        << " mid=" << mid
        << " qos_count=" << qos_count
        << " granted_qos=" << granted_qos[0]
        << "\n";
}