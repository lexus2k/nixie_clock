#include "mqtt_controller.h"
#include "platform/system.h"
#include "utils.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include <stdio.h>

//static const char *TAG = "MQTT";

static esp_mqtt_client_handle_t client = nullptr;
static uint32_t s_ts;

void mqtt_controller_init(void)
{
    s_ts = millis();
    esp_mqtt_client_config_t mqtt_cfg{};
    mqtt_cfg.uri = "mqtt://mqtt:mqtt@192.168.1.101";

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}

void mqtt_controller_run(void)
{
    if ( !client )
    {
         return;
    }
    if ( static_cast<uint32_t>( millis() - s_ts ) < 60000 )
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
        s_ts = millis();
    }
}

void mqtt_controller_deinit(void)
{
    esp_mqtt_client_stop(client);
    esp_mqtt_client_destroy(client);
    client =  nullptr;
}
