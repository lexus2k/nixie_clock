#include "config_parser.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "string.h"
#include "wifi_task.h"

static const char *TAG = "CFG";

static char *split_string(char **buffer, int *len)
{
    char *key = *buffer;
    char *p = *buffer;
    while (*len)
    {
        if (*p == 0) break;
        if (*p == '\n' || *p == '=' || *p == '&')
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

int apply_new_config(char *buffer, int len)
{
    const char *ssid = nullptr;
    const char *psk = nullptr;
    while (len)
    {
        char *key=split_string((char**)&buffer, &len);
        char *value=split_string((char**)&buffer, &len);
        ESP_LOGI(TAG, "%s=%s", key, value);
        if (!strcmp(key,"ssid") && strcmp(value, ""))
            ssid =  value;
        if (!strcmp(key,"psk") && strcmp(value, "********"))
            psk = value;
    }
    if ( app_wifi_set_sta_config(ssid, psk) < 0 )
    {
        return -1;
    }
    return 0;
}

