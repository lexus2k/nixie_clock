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

//static EventGroupHandle_t wifi_event_group;
static wifi_config_t sta_config[MAX_STA_COUNT] = {};
static bool sta_config_valid[MAX_STA_COUNT] = {};
static bool s_sta_started = false;
static bool s_ap_started = false;
static bool connected = false;
static wifi_manager_config_t config = WIFI_MANAGER_CONFIG_INIT();
static SemaphoreHandle_t xMutex = NULL;
static int retries = 0;

static int __wifi_manager_find_ap(wifi_ap_record_t *ap)
{
    int index = -1;
    for (int i=0; i<MAX_STA_COUNT; i++)
    {
        if ( sta_config_valid[i] &&
             !strncmp((char *)sta_config[i].sta.ssid, (char *)ap->ssid, sizeof(sta_config[i].sta.ssid) ) )
        {
            index = i;
            break;
        }
    }
    printf("Found index %d\n", index);
    return index;
}

static esp_err_t wifi_sta_event_handler(void *ctx, system_event_t *event)
{
    static bool connected = false;
    switch(event->event_id)
    {
//             STA MODE
        case SYSTEM_EVENT_STA_START:
            retries = 0;
//            esp_wifi_connect();
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
            if ( s_sta_started )
            {
                if ( retries < 3 )
                {
                    retries++;
                    esp_wifi_connect();
                }
                else
                {
                    wifi_scan_config_t config = { NULL, NULL, 0, true, WIFI_SCAN_TYPE_ACTIVE,
                                                 .scan_time.active = { 1000, 1000 } }; // Show hidden
                    ESP_ERROR_CHECK( esp_wifi_scan_start( &config, false ) );
                }
            }
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
                uint16_t number = MAX_STA_COUNT;
                wifi_ap_record_t ap_records[MAX_STA_COUNT];
                esp_err_t err = esp_wifi_scan_get_ap_records(&number, ap_records);
                int best_network = -1;
                int rssi = -100;
                if ( err == ESP_OK )
                {
                    for (int i=0; i<number; i++)
                    {
                        int index = __wifi_manager_find_ap(&ap_records[i]);
                        if ( ( index >= 0 ) && ( rssi < ap_records[i].rssi ) )
                        {
                            best_network = index;
                            rssi = ap_records[i].rssi;
                        }
                    }
                    if (best_network >= 0 )
                    {
                        retries = 0;
                        printf("Connecting to %d \n", best_network);
//                        esp_wifi_stop();
                        ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config[best_network] ) );
//                        esp_wifi_start();
                        ESP_ERROR_CHECK( esp_wifi_connect() );
                    }
                    else
                    {
                        wifi_scan_config_t config = { NULL, NULL, 0, true, WIFI_SCAN_TYPE_ACTIVE,
                                                 .scan_time.active = { 1000, 1000 } }; // Show hidden
                        ESP_ERROR_CHECK( esp_wifi_scan_start( &config, false ) );
                    }
                }
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

void wifi_manager_generate_ap_config( wifi_config_t *ap_config,
                                      const char *prefix,
                                      char *password )
{
    memset(ap_config, 0, sizeof(wifi_config_t));
    strncpy((char *)ap_config->ap.ssid, prefix, sizeof(ap_config->ap.ssid));
    ap_config->ap.ssid_len = strnlen((char *)ap_config->ap.ssid, sizeof(ap_config->ap.ssid));
    strncpy((char *)ap_config->ap.password,
            password ? : "00000000", sizeof(ap_config->ap.ssid) );
    ap_config->ap.authmode = WIFI_AUTH_WPA2_PSK;
    ap_config->ap.ssid_hidden = 0;
    ap_config->ap.max_connection = 5;
    ap_config->ap.beacon_interval = 100;
    uint8_t mac[6];
    if ( esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP ) == ESP_OK )
    {
        snprintf((char *)ap_config->ap.ssid + ap_config->ap.ssid_len,
                 sizeof(ap_config->ap.ssid) - ap_config->ap.ssid_len,
                 "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        ap_config->ap.ssid_len = strnlen((char *)ap_config->ap.ssid, sizeof(ap_config->ap.ssid));
    }
}

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
}

void wifi_manager_deinit(void)
{
    wifi_manager_disconnect();
    esp_wifi_deinit();
    vSemaphoreDelete( xMutex );
}


int wifi_manager_add_network(wifi_config_t *sta)
{
    int index = -1;
    xSemaphoreTake( xMutex, portMAX_DELAY );
    for(int i=0; i<MAX_STA_COUNT; i++)
    {
        if (!sta_config_valid[i])
        {
            sta_config_valid[i] = true;
            sta_config[i] = *sta;
            index = i;
            break;
        }
    }
    xSemaphoreGive( xMutex );
    return index;
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
        ESP_ERROR_CHECK( esp_wifi_connect() );
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
        wifi_scan_config_t config = { NULL, NULL, 0, true, WIFI_SCAN_TYPE_ACTIVE,
                                      .scan_time.active = { 1000, 1000 } }; // Show hidden
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
        s_sta_started = false;
        s_ap_started = false;
        esp_wifi_stop();
    }
}

int wifi_manager_network_count(void)
{
    int count = 0;
    for (int i=0; i< MAX_STA_COUNT; i++)
    {
        if ( sta_config_valid[i] ) count++;
    }
    return count;
}

void wifi_manager_reload_settings(void)
{
}

void wifi_manager_clear_settings(void)
{
}
