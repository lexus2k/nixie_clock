#include "wifi_manager.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include <string.h>

#define MAX_STA_COUNT  5

static const char* TAG = "WLAN";

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
static wifi_config_t sta_config[MAX_STA_COUNT] = {};
static bool sta_config_valid[MAX_STA_COUNT] = {};
static bool s_sta_started = false;
static bool s_ap_started = false;
static bool connected = false;
static wifi_manager_config_t config = WIFI_MANAGER_CONFIG_INIT();
static SemaphoreHandle_t xMutex = NULL;

static esp_err_t wifi_sta_event_handler(void *ctx, system_event_t *event)
{
    static bool connected = false;
    switch(event->event_id)
    {
//             STA MODE
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_STOP:
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            connected = true;
            if ( config.on_connect )
            {
                config.on_connect( false );
            }
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            if (connected)
            {
                if ( config.on_disconnect )
                {
                    config.on_disconnect( false );
                }
                connected = false;
            }
            esp_wifi_connect();
            break;
//              AP MODE
        case SYSTEM_EVENT_AP_START:
            break;
        case SYSTEM_EVENT_AP_STOP:
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            connected = true;
            if ( config.on_connect )
            {
                config.on_connect( true );
            }
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            if (connected)
            {
                if ( config.on_disconnect )
                {
                    config.on_disconnect( false );
                }
                connected = false;
            }
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            {
                ESP_LOGI(TAG, "WLAN Scan complete" );
                uint16_t number = 10;
                wifi_ap_record_t ap_records[10];
                esp_err_t err = esp_wifi_scan_get_ap_records(&number, ap_records);
                if ( err == ESP_OK )
                {
                }
                esp_wifi_scan_stop();
            }
            break;
        default:
            break;
    }
    return ESP_OK;
}

bool wifi_manager_start_ap(wifi_config_t *ap_config)
{
    wifi_manager_disconnect();
    xSemaphoreTake( xMutex, portMAX_DELAY );
    s_ap_started = true;
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, ap_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    xSemaphoreGive( xMutex );
    return true;
}

static bool __wifi_start_ap(void)
{
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
     return wifi_manager_start_ap( &ap_config );
}
/*

    ESP_LOGI(TAG, "waiting for client connection");
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, APP_WIFI_CONNECTED, pdTRUE, pdFALSE, 120000 / portTICK_PERIOD_MS);
    if ( bits & APP_WIFI_CONNECTED )
    {
        return true;
    }
    ESP_LOGI(TAG, "client is not connected in 60 seconds. disabling WiFi");
    esp_wifi_stop();
    return false; */


/*static bool __wifi_start_sta(void)
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
}*/

/*
static void wifi_manager_task(void *pvParameters)
{
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
    vTaskDelete( NULL );
}
*/

/*
void app_wifi_start(void)
{
    if ( !app_wifi_lock_control() )
    {
        ESP_LOGE(TAG, "Failed to start wifi");
        return;
    }
    xEventGroupSetBits( wifi_event_group, APP_WIFI_START );
}

void app_wifi_start_ap_only(void)
{
    if ( !app_wifi_lock_control() )
    {
        ESP_LOGE(TAG, "Failed to start wifi ap");
        return;
    }
    xEventGroupSetBits( wifi_event_group, APP_WIFI_START | APP_WIFI_START_AP );
}

void app_wifi_stop(void)
{
    if ( !app_wifi_lock_control() )
    {
        ESP_LOGE(TAG, "Failed to stop wifi");
        return;
    }
    xEventGroupSetBits( wifi_event_group, APP_WIFI_STOP );
}*/

void wifi_manager_init(wifi_manager_config_t *conf)
{
    config = *conf;
    tcpip_adapter_init();
    xMutex = xSemaphoreCreateMutex();
//    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(wifi_sta_event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_FLASH) );

    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    esp_err_t err = esp_wifi_get_config(WIFI_IF_STA, &sta_config[0]);
    if ( err == ESP_OK && sta_config[0].sta.ssid[0] != 0 )
    {
        sta_config_valid[0] = true;
    }
//    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );

/*    xTaskCreate(&wifi_manager_task,
                "wifi_manager",
                4096, NULL, 3, NULL);*/
}

void wifi_manager_deinit(void)
{
/*    if ( !app_wifi_lock_control() )
    {
        ESP_LOGE(TAG, "Failed to deinit wifi");
        return;
    }*/
//    xEventGroupSetBits( wifi_event_group, APP_WIFI_DEINIT );
//    xEventGroupWaitBits( wifi_event_group, APP_WIFI_DEINIT_COMPLETE, pdTRUE, pdFALSE, 10000 / portTICK_PERIOD_MS );
//    vEventGroupDelete( wifi_event_group );
    wifi_manager_disconnect();
    esp_wifi_deinit();
    vSemaphoreDelete( xMutex );
}


int wifi_manager_add_network(wifi_config_t *sta)
{
    return 0;
}

bool wifi_manager_modify_network(int index, wifi_config_t *conf)
{
    if ( index >= MAX_STA_COUNT )
    {
        return false;
    }
    if ( index >= 0 )
    {
        xSemaphoreTake( xMutex, portMAX_DELAY );
        sta_config[index] = *conf;
        if ( s_sta_started )
        {
            xSemaphoreGive( xMutex );
            wifi_manager_disconnect();
            // TODO: Preserve network
            return wifi_manager_connect( index );
        }
        return true;
    }
    return false;
}

bool wifi_manager_get_status(wifi_config_t *conf)
{
    xSemaphoreTake( xMutex, portMAX_DELAY );
    if ( connected && s_sta_started )
    {
        ESP_ERROR_CHECK( esp_wifi_get_config(WIFI_IF_STA, conf ) );
        xSemaphoreGive( xMutex );
        return true;
    }
    xSemaphoreGive( xMutex );
    return false;
}

bool wifi_manager_connect(int index)
{
    if ( index >= MAX_STA_COUNT )
    {
        return false;
    }
    if ( index >= 0 )
    {
        if ( !sta_config_valid[index] )
        {
             return false;
        }
        wifi_manager_disconnect();
        xSemaphoreTake( xMutex, portMAX_DELAY );
        ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
        ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config[index] ) );
        ESP_LOGI(TAG, "Using sta ssid: %s", (char *)sta_config[index].sta.ssid );
        s_sta_started = true;
        ESP_ERROR_CHECK( esp_wifi_start() );
        xSemaphoreGive( xMutex );
    }
    else if ( index < 0 )
    {
        if ( !s_sta_started )
        {
            wifi_manager_disconnect();
            ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
        }
        s_sta_started = true;
        ESP_ERROR_CHECK( esp_wifi_start() );
        wifi_scan_config_t config = { NULL, NULL, 0, true }; // Show hidden
        ESP_ERROR_CHECK( esp_wifi_scan_start( &config, false ) );
    }
    return true;
}

bool wifi_manager_get_network(int index, wifi_config_t *conf)
{
    if ( index >= MAX_STA_COUNT )
    {
        return false;
    }
    if ( index >= 0 )
    {
        if ( sta_config_valid[index] )
        {
            *conf = sta_config[index];
            return true;
        }
    }
    return false;
}

void wifi_manager_disconnect(void)
{
    if ( s_sta_started || s_ap_started )
    {
        esp_wifi_stop();
        s_sta_started = false;
        s_ap_started = false;
    }
}
