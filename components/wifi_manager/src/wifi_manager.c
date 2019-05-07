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
#define MAX_CONNECT_RETRIES_BEFORE_SCAN  20

typedef enum
{
    WM_IDLE,
    WM_READY,
    WM_CONNECTED,
} wifi_manager_state_t;

static const char* TAG = "WLAN";

//static EventGroupHandle_t wifi_event_group;
static wifi_config_t sta_config[MAX_STA_COUNT] = {};
static wifi_mode_t active_mode = WIFI_MODE_NULL;
static wifi_manager_config_t config = WIFI_MANAGER_CONFIG_INIT();
static SemaphoreHandle_t xMutex = NULL;
static wifi_manager_state_t state = WM_IDLE;

static int __wifi_manager_find_ap_by_ssid(const char *ssid)
{
    int index = -1;
    for (int i=0; i<MAX_STA_COUNT; i++)
    {
        if ( sta_config[i].sta.ssid[0] != 0 &&
             !strncmp((char *)sta_config[i].sta.ssid, ssid, sizeof(sta_config[i].sta.ssid) ) )
        {
            index = i;
            break;
        }
    }
    return index;
}

static bool __wifi_manager_start_scan(wifi_config_t *conf)
{
    wifi_scan_config_t config = { NULL, NULL, 0, true, WIFI_SCAN_TYPE_ACTIVE,
                                  .scan_time.active = { 120, 1000 } }; // Show hidden
    if ( conf != NULL )
    {
        config.ssid = conf->sta.ssid;
    }
    return esp_wifi_scan_start( &config, false ) == ESP_OK;
}

static int __wifi_manager_get_free_slot(void)
{
    int index = -1;
    for (int i=0; i< MAX_STA_COUNT; i++)
    {
        if ( sta_config[i].sta.ssid[0] == 0 )
        {
            index = i;
            break;
        }
    }
    return index;
}

static bool __wifi_manager_switch_mode(wifi_mode_t mode, wifi_config_t *conf)
{
    esp_err_t err = ESP_OK;
    if ( mode != active_mode )
    {
        if ( active_mode != WIFI_MODE_NULL )
        {
            if ( state == WM_CONNECTED )
            {
                active_mode = WIFI_MODE_NULL;
                esp_wifi_disconnect();
            }
            esp_wifi_stop();
        }
        active_mode = mode;
        if ( mode != WIFI_MODE_NULL ) // NULL is only for sniffer
        {
            ESP_ERROR_CHECK( esp_wifi_set_mode( mode ) );
        }
        if ( mode == WIFI_MODE_AP )
        {
            ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, conf ) );
            err = esp_wifi_start();
        }
        else if ( mode == WIFI_MODE_STA )
        {
            if ( conf != NULL )
            {
                ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, conf ) );
            }
            err = esp_wifi_start();
        }
    }
    return err == ESP_OK;
}

bool __wifi_manager_connect( int index, uint8_t *bssid )
{
    wifi_config_t sta = sta_config[index];
    if ( sta.sta.ssid[0] == 0 )
    {
        return false;
    }
    if ( bssid != NULL )
    {
        sta.sta.bssid_set = true;
        memcpy( sta.sta.bssid, bssid, sizeof(sta.sta.bssid) );
    }
    ESP_LOGI(TAG, "Using sta ssid: %s", (char *)sta.sta.ssid );
    __wifi_manager_switch_mode( WIFI_MODE_STA, &sta );
    esp_err_t err = esp_wifi_connect();
    return err == ESP_OK;
}

static esp_err_t wifi_sta_event_handler(void *ctx, system_event_t *event)
{
    static int retries = 0;
    switch(event->event_id)
    {
/////////////////////////////////////////             STA MODE
        case SYSTEM_EVENT_STA_START:
            state = WM_READY;
            retries = 0;
            break;
        case SYSTEM_EVENT_STA_STOP:
            state = WM_IDLE;
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            if ( state == WM_CONNECTED ) //event->event_info.got_ip.ip_changed )
            {
                // IP change?
            }
            else
            {
                state = WM_CONNECTED;
                if ( config.on_connect )
                {
                    config.on_connect( false );
                }
            }
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            if ( state == WM_CONNECTED )
            {
                if ( config.on_disconnect )
                {
                    config.on_disconnect( false );
                }
                retries = 0;
                state = WM_READY;
            }
            else
            {
                // Some connect error occured
            }
            if ( active_mode == WIFI_MODE_STA )
            {
                // This is unexpected disconnection, trying to reconnect
                if ( retries < MAX_CONNECT_RETRIES_BEFORE_SCAN )
                {
                    if ( retries == 0)
                    {
                        ESP_LOGI( TAG, "Unexpected disconnection, trying to reconnect" );
                    }
                    retries++;
                    esp_wifi_connect();
                }
                // Fallback to scan mode to find nearest network
                else
                {
                    esp_wifi_disconnect();
                    if ( retries == MAX_CONNECT_RETRIES_BEFORE_SCAN )
                    {
                        ESP_LOGI( TAG, "Reconnect failed, scanning started" );
                        retries++;
                    }
//                    vTaskDelay(1);
                    __wifi_manager_start_scan( NULL );
                }
            }
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            {
                uint16_t number = 0;
                wifi_ap_record_t *ap_records = NULL;
                esp_err_t err = esp_wifi_scan_get_ap_num( &number );
                if ( err == ESP_OK )
                {
                    ap_records = malloc( sizeof(wifi_ap_record_t) * number );
                    if ( ap_records )
                    {
                        esp_err_t err = esp_wifi_scan_get_ap_records(&number, ap_records);
                        if ( err != ESP_OK )
                        {
                            number = 0;
                        }
                    }
                    else
                    {
                        number = 0;
                    }
                }
                xSemaphoreTake( xMutex, portMAX_DELAY );
                // If we run in STA active mode, search for known networks
                if ( active_mode == WIFI_MODE_STA )
                {
                    int best_network = -1;
                    uint16_t count = 0;
                    int rssi = -100;
                    for (int i=0; i<number; i++)
                    {
                        int index = __wifi_manager_find_ap_by_ssid((char *)ap_records[i].ssid);
                        if ( index >= 0 )
                        {
                            count++;
                        }
                        if ( ( index >= 0 ) && ( rssi < ap_records[i].rssi ) )
                        {
                            best_network = index;
                            memcpy( sta_config[index].sta.bssid, ap_records[i].bssid, sizeof(sta_config[index].sta.bssid) );
                            rssi = ap_records[i].rssi;
                        }
                    }
                    ESP_LOGI(TAG, "Scan complete, found %d networks, %d known", number, count);
                    if (best_network >= 0 )
                    {
                        retries = 0;
                        ESP_LOGI( TAG, "Connecting to %s \n", (char *)sta_config[best_network].sta.ssid );
                        esp_wifi_scan_stop();
                        esp_wifi_stop();
                        ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config[best_network] ) );
                        esp_wifi_start();
                        ESP_ERROR_CHECK( esp_wifi_connect() );
                    }
                    else
                    {
                        ESP_LOGI(TAG, "Restarting scan" );
                        esp_wifi_scan_stop();
                        __wifi_manager_start_scan( NULL );
                    }
                }
                if ( ap_records )
                {
                    free( ap_records );
                }
                xSemaphoreGive( xMutex );
            }
            break;
///////////////////////////////////////////              AP MODE
        case SYSTEM_EVENT_AP_START:
            state = WM_READY;
            break;
        case SYSTEM_EVENT_AP_STOP:
            state = WM_IDLE;
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            if ( state == WM_CONNECTED )
            {
                // New client
            }
            else
            {
                state = WM_CONNECTED;
            }
            if ( config.on_connect )
            {
                config.on_connect( true );
            }
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            if ( state == WM_CONNECTED )
            {
                if ( config.on_disconnect )
                {
                    config.on_disconnect( false );
                }
                state = WM_READY;
            }
            break;
        default:
            break;
    }
    return ESP_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool wifi_manager_start_ap(wifi_config_t *ap_config)
{
    xSemaphoreTake( xMutex, portMAX_DELAY );
    bool result = __wifi_manager_switch_mode( WIFI_MODE_AP, ap_config );
    xSemaphoreGive( xMutex );
    return result;
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

int wifi_manager_add_network(wifi_config_t *sta)
{
    int index = -1;
    xSemaphoreTake( xMutex, portMAX_DELAY );
    index = __wifi_manager_get_free_slot();
    if ( index >= 0 )
    {
        sta_config[index] = *sta;
    }
    xSemaphoreGive( xMutex );
    return index;
}

bool wifi_manager_modify_network(int index, wifi_config_t *conf)
{
    bool result = false;
    xSemaphoreTake( xMutex, portMAX_DELAY );
    if ( index >= 0 && index < MAX_STA_COUNT )
    {
        sta_config[index] = *conf;
        if ( active_mode == WIFI_MODE_STA )
        {
            if ( state != WM_CONNECTED )
            {
                result = __wifi_manager_connect( index, NULL );
            }
        }
        else
        {
            result = true;
        }
    }
    xSemaphoreGive( xMutex );
    return result;
}

bool wifi_manager_get_status(wifi_config_t *conf)
{
    xSemaphoreTake( xMutex, portMAX_DELAY );
    if ( state == WM_CONNECTED && active_mode == WIFI_MODE_STA )
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
    bool result = false;
    xSemaphoreTake( xMutex, portMAX_DELAY );
    if ( index >= 0 && index < MAX_STA_COUNT )
    {
        result = __wifi_manager_connect( index, NULL );
    }
    else if ( index < 0 )
    {
        __wifi_manager_switch_mode( WIFI_MODE_STA, NULL );
        result = __wifi_manager_start_scan( NULL );
    }
    xSemaphoreGive( xMutex );
    return result;
}

bool wifi_manager_get_network(int index, wifi_config_t *conf)
{
    bool result = false;
    xSemaphoreTake( xMutex, portMAX_DELAY );
    if ( index >= 0 && index < MAX_STA_COUNT )
    {
        if ( sta_config[index].sta.ssid[0] != 0 )
        {
            *conf = sta_config[index];
            result = true;
        }
    }
    xSemaphoreGive( xMutex );
    return result;
}

void wifi_manager_disconnect(void)
{
    xSemaphoreTake( xMutex, portMAX_DELAY );
    __wifi_manager_switch_mode( WIFI_MODE_NULL, NULL );
    xSemaphoreGive( xMutex );
}

int wifi_manager_network_count(void)
{
    int count = 0;
    xSemaphoreTake( xMutex, portMAX_DELAY );
    for (int i=0; i< MAX_STA_COUNT; i++)
    {
        if ( sta_config[i].sta.ssid[0] != 0 ) count++;
    }
    xSemaphoreGive( xMutex );
    return count;
}

void wifi_manager_load_settings(void)
{
    xSemaphoreTake( xMutex, portMAX_DELAY );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_FLASH) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    if ( esp_wifi_get_config(WIFI_IF_STA, &sta_config[0]) == ESP_OK &&
         sta_config[0].sta.ssid[0] != 0 )
    {
        ESP_LOGI( TAG, "Found config in ESP32 NVS block %s", (char *)sta_config[0].sta.ssid );
    }
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );

    nvs_handle handle = 0;
    esp_err_t ret = nvs_open("wifi_mgr",
                             NVS_READONLY,
                             &handle);
    if ( ret == ESP_ERR_NVS_NOT_FOUND )
    {
        ESP_LOGI( TAG, "No AP remembered" );
        xSemaphoreGive( xMutex );
        return;
    }
    if ( ret != ESP_OK )
    {
        ESP_LOGE( TAG, "Error opening NVS storage" );
        xSemaphoreGive( xMutex );
        return;
    }
    for (int i=0; i<MAX_STA_COUNT; i++)
    {
        int index = __wifi_manager_get_free_slot();
        if ( index >= 0 )
        {
            char key[8];
            wifi_config_t config;
            size_t max_size = sizeof(wifi_config_t);
            snprintf( key, sizeof(key), "wlan%d", i );
            if ( nvs_get_blob(handle, key, &config, &max_size) == ESP_OK )
            {
                if ( config.sta.ssid[0] )
                {
                    if ( __wifi_manager_find_ap_by_ssid( (char *)config.sta.ssid ) < 0 )
                    {
                        ESP_LOGI( TAG, "Loaded STA config %s in NVS", (char *)config.sta.ssid );
                        sta_config[index] = config;
                    }
                    else
                    {
                        ESP_LOGI( TAG, "Found duplicated STA config %s in NVS", (char *)config.sta.ssid );
                    }
                }
            }
        }
    }
    nvs_close(handle);
    xSemaphoreGive( xMutex );
}

void wifi_manager_save_settings(void)
{
    xSemaphoreTake( xMutex, portMAX_DELAY );
    nvs_handle handle = 0;
    esp_err_t ret = nvs_open("wifi_mgr",
                             NVS_READWRITE,
                             &handle);
    if ( ret != ESP_OK )
    {
        ESP_LOGE( TAG, "Error opening NVS storage" );
        xSemaphoreGive( xMutex );
        return;
    }
    for (int i=0; i<MAX_STA_COUNT; i++)
    {
        char key[8];
        wifi_config_t config = sta_config[i];
        snprintf( key, sizeof(key), "wlan%d", i );
        if ( nvs_set_blob(handle, key, &config, sizeof(wifi_config_t)) != ESP_OK )
        {
            ESP_LOGE( TAG, "Failed to write WLAN slot to NVS" );
        }
        else if ( config.sta.ssid[0] != 0 )
        {
            ESP_LOGI( TAG, "Saved STA config %s to NVS", (char *)config.sta.ssid );
        }
    }
    nvs_commit(handle);
    nvs_close(handle);
    xSemaphoreGive( xMutex );
}

void wifi_manager_reload_settings(void)
{
    xSemaphoreTake( xMutex, portMAX_DELAY );
    memset( sta_config, 0, sizeof(sta_config) );
    xSemaphoreGive( xMutex );
    wifi_manager_load_settings();
}

void wifi_manager_clear_settings(void)
{
}

//////////////////////////////////////////////////////////////////////////
/////////// INIT / DEINIT
//////////////////////////////////////////////////////////////////////////

void wifi_manager_init(wifi_manager_config_t *conf)
{
    static bool tcp_initialized = false;
    state = WM_IDLE;
    config = *conf;
    xMutex = xSemaphoreCreateMutex();
    memset( sta_config, 0, sizeof(sta_config));
    if ( !tcp_initialized )
    {
        tcpip_adapter_init();
//    wifi_event_group = xEventGroupCreate();
        ESP_ERROR_CHECK( esp_event_loop_init(wifi_sta_event_handler, NULL) );
//        esp_wifi_set_auto_connect(false);
        tcp_initialized = true;
    }
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
}

void wifi_manager_deinit(void)
{
    wifi_manager_disconnect();
    esp_wifi_deinit();
    vSemaphoreDelete( xMutex );
}
