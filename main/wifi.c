#include "http_ota_server.h"

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

const int CONNECTED_BIT = BIT0;
const int STOP_REQUEST_BIT = BIT1;

static bool volatile wifi_active = false;
static EventGroupHandle_t wifi_event_group;

void start_tftp(void);
void run_tftp(void);
void stop_tftp(void);

static esp_err_t wifi_sta_event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id)
    {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            start_tftp();
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            start_tftp();
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            stop_tftp();
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            stop_tftp();
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
            break;
        default:
            break;
    }
    return ESP_OK;
}

static bool wifi_start_ap(void)
{
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );
    wifi_config_t ap_config = {
        .ap = {
            .ssid = "nixieclock",
            .password = "00000000",
            .authmode = WIFI_AUTH_WPA2_PSK,
            .ssid_hidden = 0,
            .max_connection = 1,
            .beacon_interval = 100,
        }
    };
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, &ap_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_LOGI("wifi", "waiting for client connection");
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, pdTRUE, pdFALSE, 60000 / portTICK_PERIOD_MS);
    if ( bits & CONNECTED_BIT )
    {
        return true;
    }
    ESP_LOGI("wifi", "client is not connected in 60 seconds. disabling WiFi");
    esp_wifi_stop();
    return false;
}

static bool wifi_start_sta(void)
{
    wifi_config_t sta_config = {};
    ESP_ERROR_CHECK( esp_wifi_get_config(WIFI_IF_STA, &sta_config) );
    ESP_LOGI("wifi", "stored ssid: %s", (char *)sta_config.sta.ssid );
    if ( sta_config.sta.ssid[0] == 0 )
    {
        return false; // Fallback to AP mode
    }
    ESP_LOGI("wifi", "connecting to %s", (char *)sta_config.sta.ssid );

    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    // Wait for connection
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, pdTRUE, pdFALSE, 40000 / portTICK_PERIOD_MS);
    if ( bits & CONNECTED_BIT )
    {
        return true;
    }
    ESP_LOGI("wifi", "failed to connect to %s", (char*)sta_config.sta.ssid);
    esp_wifi_stop();
    return false; // Fallback to AP mode
}

static void wifi_task(void *pvParameters)
{
    static bool first_start = true;
    if (first_start)
    {
        tcpip_adapter_init();
        wifi_event_group = xEventGroupCreate();
        ESP_ERROR_CHECK( esp_event_loop_init(wifi_sta_event_handler, NULL) );
        first_start = false;
    }
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_FLASH) );

    bool sta_mode = true;
    if (!wifi_start_sta())
    {
        sta_mode = false;
        ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
        if (!wifi_start_ap())
        {
            goto err_1;
        }
    }
//    start_tftp();
    run_tftp();
    // Only for STA
//    esp_wifi_disconnect();
    esp_wifi_stop();
err_1:
    esp_wifi_deinit();
    wifi_active = false;
    vTaskDelete( NULL );
}

void wifi_start_server(void)
{
    if (wifi_active)
    {
        ESP_LOGI("wifi", "already started");
        return;
    }
    wifi_active = true;
    xTaskCreate(&wifi_task, "wifi_task", 4096, NULL, 5, NULL);
}
