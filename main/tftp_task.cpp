#include "tftp_task.h"
#include "tftp_ota_server.h"
#include "config_parser.h"

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

int TftpConfigServer::on_write_data(uint8_t *buffer, int len)
{
    if (m_is_nvram)
    {
        if (len >= 512)
        {
            ESP_LOGE(TAG, "configs more than 511 bytes are not supported");
            return -1;
        }
        if ( apply_new_config( (char *)buffer, len ) < 0 )
        {
            return -1;
        }
        return 0;
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
