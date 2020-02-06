#include "mqtt_controller.h"
#include "platform/system.h"
#include "utils.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include <stdio.h>

static const char *TAG = "MQTT";

MqttSensorController::~MqttSensorController()
{
    end();
}

void MqttSensorController::begin(const std::string &uri, const std::string &user, const std::string &pass)
{
    if ( uri == "" )
    {
        return;
    }
    m_ts = millis();
    esp_mqtt_client_config_t mqtt_cfg{};
    mqtt_cfg.uri = uri.c_str(); //"mqtt://mqtt:mqtt@192.168.1.101"; // uri.c_str();
    if ( user != "" )
    {
        mqtt_cfg.username = user.c_str();
        mqtt_cfg.password = pass.c_str();
    }
    ESP_LOGI( TAG, "Connecting to MQTT broker: %s", mqtt_cfg.uri );
    client = esp_mqtt_client_init(&mqtt_cfg);
    if ( client != nullptr )
    {
        esp_mqtt_client_start(client);
    }
}

void MqttSensorController::update()
{
    if ( !client )
    {
         return;
    }
    if ( static_cast<uint32_t>( millis() - m_ts ) < m_interval )
    {
         return;
    }
    char str[16];
    snprintf(str, sizeof(str), "%.2f", nixie_get_temperature());
    if ( esp_mqtt_client_publish( client, "/home/hall/temperature", str, 0, 0, 0 ) < 0 )
    {
        // Nothing, supported only in 4.0 SDK
//        esp_mqtt_client_reconnect( client );
    }
    else
    {
        m_ts = millis();
    }
}

void MqttSensorController::end()
{
    if ( !client )
    {
        return;
    }
    esp_mqtt_client_stop(client);
    esp_mqtt_client_destroy(client);
    client =  nullptr;
}
