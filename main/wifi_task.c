#include "wifi_task.h"
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

// commands bits
static const int APP_WIFI_START = BIT0;
static const int APP_WIFI_STOP = BIT1;
static const int APP_WIFI_DEINIT = BIT2;
static const int APP_WIFI_READY = BIT3;
static const int APP_WIFI_UPDATE_CONFIG = BIT4;
static const int APP_WIFI_START_AP = BIT5;
// state bits
static const int APP_WIFI_DEINIT_COMPLETE = BIT6;
static const int APP_WIFI_DISCONNECTED = BIT7;
static const int APP_WIFI_CONNECTED = BIT8;

static EventGroupHandle_t wifi_event_group;
static wifi_config_t sta_config = {};
static bool update_sta_config = false;
static bool new_config_settings = false;

static esp_err_t wifi_sta_event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id)
    {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupClearBits(wifi_event_group, APP_WIFI_DISCONNECTED);
            xEventGroupSetBits(wifi_event_group, APP_WIFI_CONNECTED);
            send_app_event( EVT_WIFI_CONNECTED, 0 );
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            xEventGroupClearBits(wifi_event_group, APP_WIFI_DISCONNECTED);
            xEventGroupSetBits(wifi_event_group, APP_WIFI_CONNECTED);
            send_app_event( EVT_WIFI_CONNECTED, 1 );
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            xEventGroupClearBits(wifi_event_group, APP_WIFI_CONNECTED);
            xEventGroupSetBits(wifi_event_group, APP_WIFI_DISCONNECTED);
            send_app_event( EVT_WIFI_DISCONNECTED, 0 );
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            xEventGroupClearBits(wifi_event_group, APP_WIFI_CONNECTED);
            xEventGroupSetBits(wifi_event_group, APP_WIFI_DISCONNECTED);
            send_app_event( EVT_WIFI_DISCONNECTED, 1 );
            break;
        default:
            break;
    }
    return ESP_OK;
}

static bool __wifi_start_ap(void)
{
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );

    wifi_config_t ap_config = {
        .ap = {
            .ssid = "nc_clk",
            .ssid_len = strlen("nc_clk"),
            .password = "00000000",
            .authmode = WIFI_AUTH_WPA2_PSK,
            .ssid_hidden = 0,
            .max_connection = 1,
            .beacon_interval = 100,
        }
    };
    uint8_t mac[6];
    if ( esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP ) == ESP_OK )
    {
        snprintf((char *)ap_config.ap.ssid, sizeof(ap_config.ap.ssid),
                 "nc%02X%02X%02X%02X", mac[2], mac[3], mac[4], mac[5]);
        ap_config.ap.ssid_len = strlen((char *)ap_config.ap.ssid);
    }

    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, &ap_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_LOGI(TAG, "waiting for client connection");
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, APP_WIFI_CONNECTED, pdTRUE, pdFALSE, 120000 / portTICK_PERIOD_MS);
    if ( bits & APP_WIFI_CONNECTED )
    {
        return true;
    }
    ESP_LOGI(TAG, "client is not connected in 60 seconds. disabling WiFi");
    esp_wifi_stop();
    return false;
}

static bool __wifi_start_sta(void)
{
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    if (update_sta_config)
    {
        ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
        ESP_LOGI(TAG, "updated sta ssid: %s", (char *)sta_config.sta.ssid );
        update_sta_config = false;
        new_config_settings = false;
    }
    else
    {
        ESP_ERROR_CHECK( esp_wifi_get_config(WIFI_IF_STA, &sta_config) );
        ESP_LOGI(TAG, "nvram sta ssid: %s", (char *)sta_config.sta.ssid );
        new_config_settings = false;
    }
    if ( sta_config.sta.ssid[0] == 0 )
    {
        ESP_LOGI(TAG, "sta mode is not configured" );
        return false; // Fallback to AP mode
    }
    ESP_LOGI(TAG, "connecting to %s", (char *)sta_config.sta.ssid );

    ESP_ERROR_CHECK( esp_wifi_start() );
    // Wait for connection
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, APP_WIFI_CONNECTED, pdTRUE, pdFALSE, 40000 / portTICK_PERIOD_MS);
    if ( bits & APP_WIFI_CONNECTED )
    {
        return true;
    }
    ESP_LOGI(TAG, "failed to connect to %s", (char*)sta_config.sta.ssid);
    esp_wifi_stop();
    // No fallback to AP mode. AP is started only if STA is not configured or op key press
    return true;
}

static bool __wifi_stop(void)
{
    ESP_ERROR_CHECK( esp_wifi_stop() );
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, APP_WIFI_DISCONNECTED, pdTRUE, pdFALSE, 20000 / portTICK_PERIOD_MS);
    if ( bits & APP_WIFI_DISCONNECTED )
    {
        return true;
    }
    return false;
}

static void wifi_task(void *pvParameters)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_FLASH) );
    xEventGroupSetBits( wifi_event_group, APP_WIFI_READY | APP_WIFI_DISCONNECTED );
    while (1)
    {
        EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
             APP_WIFI_START | APP_WIFI_STOP | APP_WIFI_DEINIT | APP_WIFI_UPDATE_CONFIG,
             pdFALSE, pdFALSE, 5000 / portTICK_PERIOD_MS);
        if (bits & APP_WIFI_DEINIT)
        {
            xEventGroupClearBits( wifi_event_group, APP_WIFI_DEINIT );
            xEventGroupSetBits( wifi_event_group, APP_WIFI_DEINIT_COMPLETE );
            break;
        }
        else if (bits & APP_WIFI_STOP )
        {
            __wifi_stop();
            xEventGroupClearBits( wifi_event_group, APP_WIFI_STOP );
            xEventGroupSetBits( wifi_event_group, APP_WIFI_READY );
        }
        else if (bits & APP_WIFI_START)
        {
            if ( bits & APP_WIFI_CONNECTED )
            {
                __wifi_stop();
            }
            if ( (bits & APP_WIFI_START_AP) || !__wifi_start_sta())
            {
                if (!__wifi_start_ap())
                {
                    send_app_event( EVT_WIFI_FAILED, 0 );
                    ESP_LOGE(TAG, "failed to initialize wifi");
                }
            }
            xEventGroupClearBits( wifi_event_group, APP_WIFI_START | APP_WIFI_START_AP );
            xEventGroupSetBits( wifi_event_group, APP_WIFI_READY );
        }
        else if (bits & APP_WIFI_UPDATE_CONFIG )
        {
            vTaskDelay( 2000 / portTICK_PERIOD_MS );
            xEventGroupClearBits( wifi_event_group, APP_WIFI_UPDATE_CONFIG );
            xEventGroupSetBits( wifi_event_group, APP_WIFI_START );
            // do not set APP_WIFI_READY bit here as we're not ready yet
        }
    }
    esp_wifi_deinit();
    vTaskDelete( NULL );
}

void app_wifi_init(void)
{
    wifi_event_group = xEventGroupCreate();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(wifi_sta_event_handler, NULL) );
    xTaskCreate(&wifi_task,
                "wifi_task",
                4096, NULL, 3, NULL);
}

static bool app_wifi_wait_for_ready()
{
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, APP_WIFI_READY, pdTRUE, pdFALSE, 500 / portTICK_PERIOD_MS);
    if ( !(bits & APP_WIFI_READY) )
    {
        ESP_LOGE(TAG, "Wi-fi service is not ready to access commands");
        return false;
    }
    return true;
}

void app_wifi_done(void)
{
    if ( !app_wifi_wait_for_ready() )
    {
        ESP_LOGE(TAG, "Failed to deinit wifi");
        return;
    }
    xEventGroupSetBits( wifi_event_group, APP_WIFI_DEINIT );
    xEventGroupWaitBits( wifi_event_group, APP_WIFI_DEINIT_COMPLETE, pdTRUE, pdFALSE, 10000 / portTICK_PERIOD_MS );
    vEventGroupDelete( wifi_event_group );
}

void app_wifi_start(void)
{
    if ( !app_wifi_wait_for_ready() )
    {
        ESP_LOGE(TAG, "Failed to start wifi");
        return;
    }
    xEventGroupSetBits( wifi_event_group, APP_WIFI_START );
}

void app_wifi_start_ap_only(void)
{
    if ( !app_wifi_wait_for_ready() )
    {
        ESP_LOGE(TAG, "Failed to start wifi ap");
        return;
    }
    xEventGroupSetBits( wifi_event_group, APP_WIFI_START | APP_WIFI_START_AP );
}

void app_wifi_stop(void)
{
    if ( !app_wifi_wait_for_ready() )
    {
        ESP_LOGE(TAG, "Failed to stop wifi");
        return;
    }
    xEventGroupSetBits( wifi_event_group, APP_WIFI_STOP );
}

const char *app_wifi_get_sta_ssid(void)
{
    return (char*)sta_config.sta.ssid;
}

void app_wifi_load_settings(void)
{
    if ( !app_wifi_wait_for_ready() )
    {
        ESP_LOGE(TAG, "Failed to load settings fow Wi-Fi STA");
        return;
    }
    esp_wifi_get_config(WIFI_IF_STA, &sta_config);
    new_config_settings = false;
    // return READY flag back, we do not need to execute any command
    xEventGroupSetBits( wifi_event_group, APP_WIFI_READY );
}

int app_wifi_set_sta_ssid_psk(const char *ssid, const char *psk)
{
    if ( !app_wifi_wait_for_ready() )
    {
        ESP_LOGE(TAG, "Failed to wait for ready state");
        return -1;
    }
    if ((ssid != NULL) && strcmp((char *)sta_config.sta.ssid, ssid))
    {
        strncpy((char*)sta_config.sta.ssid, ssid, sizeof(sta_config.sta.ssid));
        new_config_settings = true;
    }
    if ((psk != NULL) && strcmp((char *)sta_config.sta.password, psk))
    {
        strncpy((char*)sta_config.sta.password, psk, sizeof(sta_config.sta.password));
        new_config_settings = true;
    }
    // return READY flag back, we do not need to execute any command
    xEventGroupSetBits( wifi_event_group, APP_WIFI_READY );
    return 0;
}

int app_wifi_apply_sta_settings(void)
{
    if ( !app_wifi_wait_for_ready() )
    {
        ESP_LOGE(TAG, "Failed to wait for ready state");
        return -1;
    }
    if (new_config_settings)
    {
        update_sta_config = true;
        new_config_settings = false;
    }
    if ( update_sta_config )
    {
        xEventGroupSetBits( wifi_event_group, APP_WIFI_UPDATE_CONFIG );
    }
    else
    {
        // return READY flag back, we do not need to execute any command
        xEventGroupSetBits( wifi_event_group, APP_WIFI_READY );
    }
    return 0;
}
