#include "clock_settings.h"
#include "clock_hardware.h"
#include "clock_time.h"
#include "version.h"

static const char *TAG = "CFG";

ClockSettings::ClockSettings()
    : NvsSettings("nixie")
    , m_modified(false)
    , m_tz("VLAT-10:00:00")
    , m_color( 0x00007F00 )
    , m_night_mode(false)
    , m_day_brightness( 160 )
    , m_night_brightness( 64 )
    , m_day_time  ( 0x00080000 )
    , m_night_time( 0x00150000 )
    , m_time_auto( true )
{
}

void ClockSettings::save()
{
    if (m_modified)
    {
        begin(NVS_READWRITE);
        set("tz", m_tz, sizeof(m_tz));
        set("color", m_color);
        set("nm", m_night_mode);
        set("dbr", m_day_brightness);
        set("nbr", m_night_brightness);
        set("dt", m_day_time);
        set("nt", m_night_time);
        set("ta", m_time_auto);
        end();
    }
    m_modified = false;
}

void ClockSettings::load()
{
    begin(NVS_READONLY);
    get("tz", m_tz, sizeof(m_tz));
    get("color", m_color);
    get("nm", m_night_mode);
    get("dbr", m_day_brightness);
    get("nbr", m_night_brightness);
    get("dt", m_day_time);
    get("nt", m_night_time);
    get("ta", m_time_auto);
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

bool ClockSettings::get_night_mode()
{
    return m_night_mode;
}

void ClockSettings::set_night_mode(bool enable)
{
    m_night_mode = enable;
    m_modified = true;
}

bool ClockSettings::get_time_auto()
{
    return m_night_mode;
}

void ClockSettings::set_time_auto(bool enable)
{
    m_time_auto = enable;
    m_modified = true;
}

uint8_t ClockSettings::get_night_brightness()
{
    return m_night_brightness;
}

void ClockSettings::set_night_brightness(uint8_t value)
{
    m_night_brightness = value;
    m_modified = true;
}

uint8_t ClockSettings::get_day_brightness()
{
    return m_day_brightness;
}

void ClockSettings::set_day_brightness(uint8_t value)
{
    m_day_brightness = value;
    m_modified = true;
}

void ClockSettings::set_day_time(struct tm *tm_info)
{
    m_day_time = pack_time_u32(tm_info);
    m_modified = true;
}

struct tm ClockSettings::get_day_time()
{
    struct tm tm_info;
    unpack_time_u32(&tm_info, m_day_time);
    return tm_info;
}

void ClockSettings::set_night_time(struct tm *tm_info)
{
    m_night_time = pack_time_u32(tm_info);
    m_modified = true;
}

struct tm ClockSettings::get_night_time()
{
    struct tm tm_info;
    unpack_time_u32(&tm_info, m_night_time);
    return tm_info;
}

/////////////////////////////////////////////////////////////////////////////

#include "clock_display.h"
#include "wifi_task.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "lwip/apps/sntp.h"

bool wifi_is_up = false;

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
            strftime(data, max_len, "%Y-%m-%d", tm_info);
        }
    }
    else if (!strcmp(param, "time"))
    {
        struct timeval tv;
        if ( gettimeofday(&tv, NULL) == 0)
        {
            struct tm* tm_info;
            tm_info = localtime(&tv.tv_sec);
            local_time_to_web( data, max_len, tm_info );
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
    else if (!strcmp(param, "day_br"))
    {
        snprintf(data, max_len, "%d", settings.get_day_brightness());
    }
    else if (!strcmp(param, "night_br"))
    {
        snprintf(data, max_len, "%d", settings.get_night_brightness());
    }
    else if (!strcmp(param, "night_mode"))
    {
        snprintf(data, max_len, "%s", settings.get_night_mode() ? "on": "off");
    }
    else if (!strcmp(param, "time_auto"))
    {
        snprintf(data, max_len, "%s", settings.get_time_auto() ? "on": "off");
    }
    else if (!strcmp(param, "night_time"))
    {
        struct tm tm_info = settings.get_night_time();
        local_time_to_web( data, max_len, &tm_info );
    }
    else if (!strcmp(param, "day_time"))
    {
        struct tm tm_info = settings.get_day_time();
        local_time_to_web( data, max_len, &tm_info );
    }
    else if (!strcmp(param, "ver"))
    {
        strncpy( data, FW_VERSION, max_len );
        data[max_len-1] = '\0';
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
    ESP_LOGI( TAG, "trying: %s=%s", param, data);
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
    else if (!strcmp(param, "day_br"))
    {
        uint8_t brightness = strtoul(data, nullptr, 10);
        settings.set_day_brightness( brightness );
        display.set_brightness( brightness );
        leds.set_brightness( brightness );
    }
    else if (!strcmp(param, "night_br"))
    {
        uint8_t brightness = strtoul(data, nullptr, 10);
        settings.set_night_brightness( brightness );
        display.set_brightness( brightness );
        leds.set_brightness( brightness );
    }
    else if (!strcmp(param, "night_mode"))
    {
        bool mode = (!strcmp(data, "on")) ? true: false;
        settings.set_night_mode( mode );
    }
    else if (!strcmp(param, "time_auto"))
    {
        bool mode = (!strcmp(data, "on")) ? true: false;
        settings.set_time_auto( mode );
        if ( mode )
        {
            if (wifi_is_up) sntp_init();
        }
        else
        {
            if (sntp_enabled()) sntp_stop();
        }
    }
    else if (!strcmp(param, "day_time"))
    {
        struct tm tm_info;
        web_time_to_local( data, &tm_info );
        settings.set_day_time( &tm_info );
    }
    else if (!strcmp(param, "night_time"))
    {
        struct tm tm_info;
        web_time_to_local( data, &tm_info );
        settings.set_night_time( &tm_info );
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
        if (sntp_enabled() && settings.get_time_auto())
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
    apply_settings();
    return 0;
}

int apply_settings()
{
    if ( is_night_time() )
    {
        display.set_brightness( settings.get_night_brightness() );
        leds.set_brightness( settings.get_night_brightness() );
    }
    else
    {
        display.set_brightness( settings.get_day_brightness() );
        leds.set_brightness( settings.get_day_brightness() );
    }
    return 0;
}

int is_night_time()
{
    int night_time = 0;
    if (settings.get_night_mode())
    {
        struct tm tm_info;
        struct tm tm_dayinfo = settings.get_day_time();
        struct tm tm_nightinfo = settings.get_night_time();
        get_current_time( &tm_info );
        uint16_t current_min = daytime_to_minutes( &tm_info );
        uint16_t day_min = daytime_to_minutes( &tm_dayinfo );
        uint16_t night_min = daytime_to_minutes( &tm_nightinfo );
        if ( (current_min > night_min) || (current_min < day_min ) )
        {
           night_time = 1;
        }
    }
    return night_time;
}

int powered_on = 1;

void power_off()
{
    powered_on = 0;
    display.off();
    leds.disable();
}

void power_on()
{
    powered_on = 1;
    display.on();
    leds.enable();
}

int is_power_on()
{
    return powered_on;
}
