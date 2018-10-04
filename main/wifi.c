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

static bool volatile wifi_created = false;
static EventGroupHandle_t wifi_event_group;

static esp_err_t wifi_sta_event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id)
    {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
            break;
        default:
            break;
    }
    return ESP_OK;
}


static bool wifi_start_sta(void)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_APSTA) );
    wifi_config_t sta_config = {
        .sta = {
            .ssid = "Apt5S",
            .password = "password",
            .bssid_set = false
        }
    };
    wifi_config_t ap_config = {
        .ap = {
            .ssid = "clock_n",
            .password = "nixieclock",
            .authmode = WIFI_AUTH_WPA2_PSK,
            .ssid_hidden = 0,
            .max_connection = 1,
            .beacon_interval = 100,
        }
    };
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, &ap_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, pdTRUE, pdFALSE, 60000 / portTICK_PERIOD_MS);
    if ( bits & CONNECTED_BIT )
    {
        return true;
    }
    ESP_LOGI("wifi", "failed to start STA mode");
    esp_wifi_stop();
    esp_wifi_deinit();
    return false;
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

    if (!wifi_start_sta())
    {
        wifi_created = false;
        vTaskDelete( NULL );
    }

    // Only for STA
    esp_wifi_disconnect();
    esp_wifi_stop();
    esp_wifi_deinit();

    wifi_created = false;
    vTaskDelete( NULL );
}

void wifi_start_server(void)
{
    if (wifi_created)
    {
        ESP_LOGI("wifi", "already started");
        return;
    }
    xTaskCreate(&wifi_task, "wifi_task", 4096, NULL, 5, NULL);
    wifi_created = true;
}