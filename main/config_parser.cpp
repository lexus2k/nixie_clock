#include "config_parser.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "string.h"
#include "wifi_task.h"
#include <sys/time.h>
#include <time.h>

static const char *TAG = "CFG";

static void update_date_time(const char *new_date, const char *new_time)
{
    if ( (new_date == nullptr) && (new_time == nullptr) ) return;
    struct timeval tv;
    if ( gettimeofday(&tv, nullptr) != 0)
    {
        return;
    }
    struct tm tm_info;
    localtime_r(&tv.tv_sec, &tm_info);
    if ( new_date != nullptr )
    {
        tm_info.tm_year = strtoul(new_date, nullptr, 10) - 1900;
        tm_info.tm_mon = strtoul(new_date+5, nullptr, 10);
        tm_info.tm_mday = strtoul(new_date+8, nullptr, 10);
    }
    if ( new_time != nullptr )
    {
        tm_info.tm_hour = strtoul(new_time, nullptr, 10);
        tm_info.tm_min = strtoul(new_time+3, nullptr, 10);
        tm_info.tm_sec = 0;
    }
    time_t t = mktime( &tm_info );
    tv.tv_sec = t;
    settimeofday( &tv, nullptr );
    ESP_LOGI( TAG, "Setting time/date to %s", ctime( &t ));
}

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
    const char *set_time = nullptr;
    const char *set_date = nullptr;
    while (len)
    {
        char *key=split_string((char**)&buffer, &len);
        char *value=split_string((char**)&buffer, &len);
        ESP_LOGI(TAG, "%s=%s", key, value);
        if (!strcmp(key,"ssid") && strcmp(value, ""))
            ssid =  value;
        if (!strcmp(key,"psk") && strcmp(value, "********"))
            psk = value;
        if (!strcmp(key,"set_time") && strcmp(value, ""))
            set_time = value;
        if (!strcmp(key,"set_date") && strcmp(value, ""))
            set_date = value;
    }
    if ( app_wifi_set_sta_config(ssid, psk) < 0 )
    {
        return -1;
    }
    update_date_time( set_date, set_time );
    return 0;
}

