#include "clock_settings.h"
#include "clock_hardware.h"
#include "clock_time.h"
#include "version.h"
#include "wifi_task.h"
#include "nixie_melodies.h"

static const char *TAG = "CFG";

ClockSettings::ClockSettings()
    : NvsSettings("nixie")
    , m_factory("ft_nvs", "device_data")
    , m_modified(false)
    , m_tz("VLAT-10:00:00")
    , m_log_ip_addr("")
    , m_color( 0x00007F00 )
    , m_night_mode(false)
    , m_day_brightness( 160 )
    , m_night_brightness( 64 )
    , m_day_time  ( 0x00080000 )
    , m_night_time( 0x00150000 )
    , m_time_auto( true )
    , m_brightness_auto( false )
{
}

void ClockSettings::save()
{
    if (m_modified)
    {
        begin(NVS_READWRITE);
        set("tz", m_tz, sizeof(m_tz));
        set("log_ip", m_log_ip_addr, sizeof(m_log_ip_addr));
        set("color", m_color);
        set("nm", m_night_mode);
        set("dbr", m_day_brightness);
        set("nbr", m_night_brightness);
        set("dt", m_day_time);
        set("nt", m_night_time);
        set("ta", m_time_auto);
        set("ba", m_brightness_auto);
        end();
    }
    m_modified = false;
}

void ClockSettings::load_factory()
{
    m_factory.load();
}

void ClockSettings::load()
{
    begin(NVS_READONLY);
    get("tz", m_tz, sizeof(m_tz));
    get("log_ip", m_log_ip_addr, sizeof(m_log_ip_addr));
    get("color", m_color);
    get("nm", m_night_mode);
    get("dbr", m_day_brightness);
    get("nbr", m_night_brightness);
    get("dt", m_day_time);
    get("nt", m_night_time);
    get("ta", m_time_auto);
    get("ba", m_brightness_auto);
    end();
    m_modified = false;
}

const FactorySettings& ClockSettings::factory()
{
    return m_factory;
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

const char *ClockSettings::get_log_ip_addr()
{
    return m_log_ip_addr;
}

void ClockSettings::set_log_ip_addr(const char *value)
{
    strncpy(m_log_ip_addr, value, sizeof(m_log_ip_addr));
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
    return m_time_auto;
}

void ClockSettings::set_time_auto(bool enable)
{
    m_time_auto = enable;
    m_modified = true;
}

bool ClockSettings::get_brightness_auto()
{
    return m_brightness_auto;
}

void ClockSettings::set_brightness_auto(bool auto_brightness)
{
    m_brightness_auto = auto_brightness;
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

void ClockSettings::set_highlight_enable(bool enable)
{
    m_highlight_enabled = enable;
}

bool ClockSettings::get_highlight_enable()
{
    return m_highlight_enabled;
}

void ClockSettings::set_predefined_color(int index)
{
    if (index > 7) index = 0;
    m_predefined_color = index;
    switch (index)
    {
        case 0: set_color( 0x00C00000 ); break; // red
        case 1: set_color( 0x0000C000 ); break; // green
        case 2: set_color( 0x000000C0 ); break; // blue
        case 3: set_color( 0x0000C0C0 ); break; // cyan
        case 4: set_color( 0x00C000C0 ); break; // violet
        case 5: set_color( 0x00C0C000 ); break; // yellow
        case 6: set_color( 0x00C07F00 ); break; // orange
        case 7: set_color( 0x00C0C0C0 ); break; // white
        default: set_color( 0x00C0C0C0 ); break;
    }
}

int ClockSettings::get_predefined_color()
{
    return m_predefined_color;
}

/////////////////////////////////////////////////////////////////////////////

#include "udp_logging.h"
#include "clock_display.h"
#include "wifi_task.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "lwip/apps/sntp.h"

bool wifi_is_up = false;

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
    else if (!strcmp(param, "log_ip"))
    {
        strncpy(data, settings.get_log_ip_addr(), max_len);
    }
    else if (!strcmp(param, "color"))
    {
        snprintf(data, max_len, "#%06X", settings.get_color());
    }
    else if (!strcmp(param, "red"))
    {
        snprintf(data, max_len, "%d", (settings.get_color()>>16) & 0xFF);
    }
    else if (!strcmp(param, "green"))
    {
        snprintf(data, max_len, "%d", (settings.get_color()>>8) & 0xFF);
    }
    else if (!strcmp(param, "blue"))
    {
        snprintf(data, max_len, "%d", (settings.get_color()>>0) & 0xFF);
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
        snprintf(data, max_len, "%s", settings.get_night_mode() ? "checked": "");
    }
    else if (!strcmp(param, "time_auto"))
    {
        snprintf(data, max_len, "%s", settings.get_time_auto() ? "checked": "");
    }
    else if (!strcmp(param, "br_auto"))
    {
        snprintf(data, max_len, "%s", settings.get_brightness_auto() ? "checked": "");
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
    else if (!strcmp(param, "serial"))
    {
        strncpy( data, settings.factory().get_serial_number(), max_len );
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
    else if (!strcmp(param, "br_auto"))
    {
        bool mode = (!strcmp(data, "on")) ? true: false;
        settings.set_brightness_auto( mode );
        apply_settings();
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
    else if (!strcmp(param, "log_ip"))
    {
        // TODO:
        settings.set_log_ip_addr( data );
        udp_logging_init( data, 1337, nullptr );
    }
    else if (!strcmp(param,"ssid") && strcmp(data, ""))
    {
        return app_wifi_set_sta_ssid_psk(data, nullptr);
    }
    else if (!strcmp(param,"psk") && strcmp(data, "********"))
    {
        return app_wifi_set_sta_ssid_psk(nullptr, data);
    }
    else if (!strcmp(param,"melody"))
    {
        if (!strcmp(data, "monkey"))
            audio_player.play( &melodyMonkeyIslandP );
        if (!strcmp(data, "mario"))
            audio_player.play( &melodyMario2 );
    }
    else
    {
        return -1;
    }
    return 0;
}


int load_settings()
{
    app_wifi_load_settings();
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
    uint8_t brightness;
    if ( settings.get_brightness_auto() )
    {
        int als_data = als.get_raw_avg();
        if ( als_data >= 0 )
        {
            brightness = 255 * als.get_raw_avg() / 1023;
            if (brightness < 3) brightness = 3;
        }
        else
        {
            // ALS is not supported on this type of board
            brightness = settings.get_day_brightness();
        }
    }
    else
    {
        if ( is_night_time() )
        {
            brightness = settings.get_night_brightness();
        }
        else
        {
            brightness = settings.get_day_brightness();
        }
    }
    display.set_brightness( brightness );
    leds.set_brightness( brightness );
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

