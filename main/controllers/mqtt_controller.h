#pragma once

#include "mqtt_client.h"
#include <stdint.h>
#include <string>

class MqttSensorController
{
public:
    MqttSensorController() = default;
    ~MqttSensorController();

    void begin(const std::string &uri, const std::string &user = "", const std::string &pass="");
    void update();
    void end();

private:
    esp_mqtt_client_handle_t client = nullptr;
    uint32_t m_ts = 0;
    uint32_t m_interval = 60 * 1000;
};

