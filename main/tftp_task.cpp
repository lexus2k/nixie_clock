#include "tftp_task.h"
#include "tftp_ota_server.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "string.h"

static const char *TAG = "TFTP";
static const char *NVRAM_FILE = "nvram.txt";

int TftpConfigServer::on_write(const char *file)
{
    if (strcmp(file, NVRAM_FILE))
    {
        m_is_nvram = 0;
        return TftpOtaServer::on_write(file);
    }
    m_is_nvram = 1;
    return 1;
}

int TftpConfigServer::on_read(const char *file)
{
    if (strcmp(file, NVRAM_FILE))
    {
        m_is_nvram = 0;
        return TftpOtaServer::on_read(file);
    }
    m_is_nvram = 1;
    return 1;
}

static char *split_string(char **buffer, int *len)
{
    char *key = *buffer;
    char *p = *buffer;
    while (*len)
    {
        if (*p == 0) break;
        if (*p == '\n' || *p == '=')
        {
            break;
        }
        if (*p == '\r')
        {
            *p = 0;
        }
        p++;
        (*len)--;
    }
    *p = 0;
    if (*len)
    {
        (*len)--;
        p++;
    }
    *buffer = p;
    return key;
}

int TftpConfigServer::on_write_data(uint8_t *buffer, int len)
{
    if (m_is_nvram)
    {
        if (len >= 512)
        {
            ESP_LOGE(TAG, "configs more than 511 bytes are not supported");
            return -1;
        }
        wifi_config_t sta_config;
        if ( esp_wifi_get_config(WIFI_IF_STA, &sta_config) != ESP_OK )
        {
            return -1;
        }
        while (len)
        {
            char *key=split_string((char**)&buffer, &len);
            char *value=split_string((char**)&buffer, &len);
            ESP_LOGI(TAG, "%s=%s", key, value);
            if (!strcmp(key,"ssid"))
                strncpy((char*)sta_config.sta.ssid, value, sizeof(sta_config.sta.ssid));
            if (!strcmp(key,"psk"))
                strncpy((char*)sta_config.sta.password, value, sizeof(sta_config.sta.password));
        }
        if ( esp_wifi_set_config(WIFI_IF_STA, &sta_config) != ESP_OK )
        {
            return -1;
        }
    }
    return TftpOtaServer::on_write_data(buffer, len);
}
 
int TftpConfigServer::on_read_data(uint8_t *buffer, int len)
{
    if (m_is_nvram)
    {
        wifi_config_t sta_config;
        if ( esp_wifi_get_config(WIFI_IF_STA, &sta_config) != ESP_OK )
        {
            return 0;
        }
        return snprintf((char*)buffer, len, "ssid=%s\r\npsk=********\r\n", (char*)sta_config.sta.ssid);
    }
    return TftpOtaServer::on_read_data(buffer, len);
}

TftpConfigServer server;

static void tftp_task(void *pvParameters)
{
    while (server.run()>=0)
    {
    }
    vTaskDelete( NULL );
}


void start_tftp(void)
{
    if (server.start()<0)
    {
        return;
    }
    xTaskCreate(&tftp_task, "tftp_task", 4096, NULL, 5, NULL);
}

void stop_tftp(void)
{
    server.stop();
}
