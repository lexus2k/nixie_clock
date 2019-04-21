#include "wifi_task.h"
#include "wifi_manager.h"
#include "clock_events.h"
#include "clock_settings.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include <string.h>

static const char* TAG = "WIFI";

void on_connect(bool isAp)
{
    send_app_event( EVT_WIFI_CONNECTED, isAp ? 1: 0 );
}

void on_disconnect(bool isAp)
{
    send_app_event( EVT_WIFI_DISCONNECTED, isAp ? 1: 0 );
}

void app_wifi_init(void)
{
    wifi_manager_config_t config = WIFI_MANAGER_CONFIG_INIT();
    config.on_connect = on_connect;
    config.on_disconnect = on_disconnect;
    wifi_manager_init(&config);
    wifi_manager_load_settings();
}

void app_wifi_done(void)
{
    wifi_manager_deinit();
}

void app_wifi_start(void)
{
    if ( wifi_manager_network_count() == 0 )
    {
        wifi_config_t ap_config = {};
        wifi_manager_generate_ap_config(&ap_config, "NixieClock", NULL);
        wifi_manager_start_ap(&ap_config);
    }
    else
    {
        wifi_manager_connect( -1 );
    }
}

void app_wifi_start_ap_only(void)
{
    wifi_config_t ap_config = {};
    wifi_manager_generate_ap_config(&ap_config, "NixieClock", NULL);
    wifi_manager_start_ap(&ap_config);
}

void app_wifi_stop(void)
{
    wifi_manager_disconnect();
}

static wifi_config_t sta_config;
const char *app_wifi_get_sta_ssid(void)
{
    wifi_manager_get_network( 0, &sta_config );
    return (char*)sta_config.sta.ssid;
}

void app_wifi_load_settings(void)
{
    wifi_manager_reload_settings();
}

int app_wifi_set_sta_ssid_psk(const char *ssid, const char *psk)
{
    wifi_config_t sta_config = {};
    if ( wifi_manager_network_count() != 0 )
    {
        wifi_manager_get_network( 0, &sta_config );
    }
    if (ssid != NULL)
    {
        strncpy((char*)sta_config.sta.ssid, ssid, sizeof(sta_config.sta.ssid));
    }
    if (psk != NULL)
    {
        strncpy((char*)sta_config.sta.password, psk, sizeof(sta_config.sta.password));
    }
    if ( wifi_manager_network_count() != 0 )
    {
        wifi_manager_modify_network( 0, &sta_config );
    }
    else
    {
        wifi_manager_add_network( &sta_config );
    }
    return 0;
}

int app_wifi_apply_sta_settings(void)
{
    wifi_manager_save_settings();
    wifi_manager_connect( -1 );
    return 0;
}
