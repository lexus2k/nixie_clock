#include "clock_settings.h"
#include "clock_hardware.h"

static const char *TAG = "CFG";

ClockSettings::ClockSettings()
    : NvsSettings("nixie")
    , m_modified(false)
    , m_tz("VLAT-10:00:00")
    , m_color( 0x00007F00 )
{
}

void ClockSettings::save()
{
    if (m_modified)
    {
        begin(NVS_READWRITE);
        set("tz", m_tz, sizeof(m_tz));
        set("color", m_color);
        end();
    }
    m_modified = false;
}

void ClockSettings::load()
{
    begin(NVS_READONLY);
    get("tz", m_tz, sizeof(m_tz));
    get("color", m_color);
    end();
    m_modified = false;
}

const char *ClockSettings::get_tz()
{
    return m_tz;
}

void ClockSettings::set_tz(const char *value)
{
    strncpy(m_tz, value, sizeof(m_tz));
    m_modified = true;
}


uint32_t ClockSettings::get_color()
{
    return m_color;
}

void ClockSettings::set_color(uint32_t value)
{
    m_color = value;
    m_modified = true;
}

/////////////////////////////////////////////////////////////////////////////

#include "clock_display.h"
#include "wifi_task.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "lwip/apps/sntp.h"
#include <sys/time.h>
#include <time.h>

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

int apply_new_wifi_config(char *buffer, int len)
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

const char *settings_get_tz()
{
    return settings.get_tz();
}

int get_config_value(const char *param, char *data, int max_len)
{
    if (!strcmp(param,"revert"))
    {
        reset_settings();
        load_settings();
    }
    else if (!strcmp(param,"ssid"))
    {
        strncpy(data, app_wifi_get_sta_ssid(), max_len);
    }
    else if (!strcmp(param, "datetime"))
    {
        time_t t = time( NULL );
        strncpy(data, ctime( &t ), max_len);
    }
    else if (!strcmp(param, "date"))
    {
        struct timeval tv;
        if ( gettimeofday(&tv, NULL) == 0)
        {
            struct tm* tm_info;
            tm_info = localtime(&tv.tv_sec);
            strftime(data, max_len, "%Y/%m/%d", tm_info);
        }
    }
    else if (!strcmp(param, "time"))
    {
        struct timeval tv;
        if ( gettimeofday(&tv, NULL) == 0)
        {
            struct tm* tm_info;
            tm_info = localtime(&tv.tv_sec);
            strftime(data, max_len, "%H:%M:%S", tm_info);
        }
    }
    else if (!strcmp(param, "timezone"))
    {
        char *tz = getenv("TZ");
        strncpy(data, tz ? tz : "None", max_len);
    }
    else if (!strcmp(param, "color"))
    {
        snprintf(data, max_len, "#%06X", settings.get_color());
    }
    else
    {
        strncpy(data, "", max_len);
        return -1;
    }
    return 0;
}

int try_config_value(const char *param, char *data, int max_len)
{
    if (!strcmp(param, "color"))
    {
        uint32_t new_color = strtoul(&data[1], nullptr, 16);
        leds.set_color( new_color );
        settings.set_color( new_color );
    }
    else if (!strcmp(param, "brightness"))
    {
        uint8_t brightness = strtoul(data, nullptr, 10);
        display.set_brightness( brightness );
    }
    else if (!strcmp(param, "time") && strcmp(data, ""))
    {
        update_date_time( nullptr, data );
    }
    else if (!strcmp(param, "date") && strcmp(data, ""))
    {
        update_date_time( data, nullptr );
    }
    else if (!strcmp(param, "timezone") && strcmp(data, getenv("TZ")))
    {
        settings.set_tz(data);
        setenv("TZ", data, 1); // https://www.systutorials.com/docs/linux/man/3-tzset/
        tzset();
        if (sntp_enabled())
        {
            sntp_stop();
            sntp_init();
        }
    }
    else
    {
        return -1;
    }
    return 0;
}


int load_settings()
{
    settings.load();
    return 0;
}

int reset_settings()
{
    return 0;
}

int save_settings()
{
    settings.save();
    return 0;
}
