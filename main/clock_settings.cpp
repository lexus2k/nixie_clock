#include "clock_settings.h"
#include "clock_events.h"
#include "clock_audio.h"
#include "clock_hardware.h"
#include "clock_time.h"
#include "version.h"
#include "wifi_task.h"
#include "nixie_melodies.h"
#include "utils.h"
#include <algorithm>

static const char *TAG = "CFG";

ClockSettings::ClockSettings()
    : NvsSettings("nixie")
    , m_factory("ft_nvs", "device_data")
    , m_tz("VLAT-10:00:00")
    , m_user("admin")
    , m_pass("password")
{
}

void ClockSettings::save()
{
    if (m_modified)
    {
        begin(NVS_READWRITE);
        set("tz", m_tz, sizeof(m_tz));
        set("user", m_user, sizeof(m_user));
        set("pass", m_pass, sizeof(m_pass));
        set("color", m_color);
        set("colormode", m_color_mode);
        set("nm", m_night_mode);
        set("dbr", m_day_brightness);
        set("nbr", m_night_brightness);
        set("dt", m_day_time);
        set("nt", m_night_time);
        set("ta", m_time_auto);
        set("ba", m_brightness_auto);
        set("mqtt", m_mqtt, sizeof(m_mqtt));
        set("alarm", m_alarm );
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
    get("user", m_user, sizeof(m_user));
    get("pass", m_pass, sizeof(m_pass));
    get("color", m_color);
    get("colormode", m_color_mode);
    get("nm", m_night_mode);
    get("dbr", m_day_brightness);
    get("nbr", m_night_brightness);
    get("dt", m_day_time);
    get("nt", m_night_time);
    get("ta", m_time_auto);
    get("ba", m_brightness_auto);
    get("mqtt", m_mqtt, sizeof(m_mqtt));
    get("alarm", m_alarm );
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
    m_tz[0] = '\0';
    strncat(m_tz, value, sizeof(m_tz) - 1);
    m_modified = true;
}

const char *ClockSettings::get_user()
{
    return m_user;
}

void ClockSettings::set_user(const char *value)
{
    m_user[0] = '\0';
    strncat(m_user, value, sizeof(m_user) - 1);
    m_modified = true;
}

const char *ClockSettings::get_pass()
{
    return m_pass;
}

void ClockSettings::set_pass(const char *value)
{
    m_pass[0] = '\0';
    strncat(m_pass, value, sizeof(m_pass)-1);
    m_modified = true;
}

const char *ClockSettings::get_mqtt()
{
    return m_mqtt;
}

void ClockSettings::set_mqtt(const char *value)
{
    if ( *value && *value == '"' ) value++;
    strncpy(m_mqtt, value, sizeof(m_mqtt));
    if ( strlen(m_mqtt) > 0 && m_mqtt[strlen(m_mqtt) - 1] == '"' )
    {
        m_mqtt[strlen(m_mqtt) - 1] = '\0';
    }
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

uint8_t ClockSettings::get_color_mode()
{
    return m_color_mode;
}

void ClockSettings::set_color_mode(uint8_t value)
{
    m_color_mode = value;
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

void ClockSettings::set_alarm( uint32_t value )
{
    m_modified = true;
    m_alarm = value;
}

uint32_t ClockSettings::get_alarm()
{
    return m_alarm;
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

#include "clock_display.h"
#include "wifi_task.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "lwip/apps/sntp.h"

bool wifi_sta_is_up = false;
static int wifi_index = 0;
static int track_index = 0;

static int get_next_track(const char *name, char *value, int max_len, void *user_data )
{
    track_index++;
    if ( track_index < audio_track_get_count() )
    {
        strncpy(value, " ", max_len);
    }
    else
    {
        strncpy(value, "", max_len);
    }
    return 0;
}

static int get_date_config_value(const char *name, char *value, int max_len, void *user_data )
{
    struct timeval tv;
    if ( gettimeofday(&tv, NULL) == 0)
    {
        struct tm* tm_info;
        tm_info = localtime(&tv.tv_sec);
        strftime(value, max_len, "%Y-%m-%d", tm_info);
    }
    return 0;
}

static int get_time_config_value(const char *name, char *value, int max_len, void *user_data )
{
    struct timeval tv;
    if ( gettimeofday(&tv, NULL) == 0)
    {
        struct tm* tm_info;
        tm_info = localtime(&tv.tv_sec);
        local_time_to_web( value, max_len, tm_info );
    }
    return 0;
}

static int set_timezone_config_value(const char *name, const char *value, int max_len, void *user_data )
{
    if (strcmp(value, getenv("TZ")))
    {
        settings.set_tz(value);
        setenv("TZ", value, 1); // https://www.systutorials.com/docs/linux/man/3-tzset/
        tzset();
        if (sntp_enabled() && settings.get_time_auto())
        {
            sntp_stop();
            sntp_init();
        }
    }
    return 0;
}

static int send_button_event(const char *name, const char *value, int max_len, void *user_data )
{
    uint8_t button = strtoul(value, nullptr, 10);
    if ( button > 9 )
        send_app_event( EVT_BUTTON_LONG_HOLD, button - 11 );
    else
        send_app_event( EVT_BUTTON_PRESS, button - 1 );
    return 0;
}

static int set_timeauto_config_value(const char *name, const char *value, int max_len, void *user_data )
{
    bool mode = (!strcmp(value, "on")) ? true: false;
    settings.set_time_auto( mode );
    if ( mode )
    {
        if (wifi_sta_is_up) sntp_init();
    }
    else
    {
        if (sntp_enabled()) sntp_stop();
    }
    return 0;
}

static int get_unknown_value(const char *param, char *value, int max_len)
{
    ESP_LOGI(TAG, "Unprocessed read parameter %s", param);
    strncpy(value, "", max_len);
    return -1;
}

static int set_unknown_value(const char *param, const char *data, int max_len)
{
    ESP_LOGI(TAG, "Unprocessed write parameter %s", param);
    return -1;
}

const applet_param_t config_params[] =
{
    { "revert", nullptr, APPLET_INLINE_R(reset_settings(); load_settings(); return 0;) },
    { "reset_track", nullptr, APPLET_INLINE_R(track_index=0; value[0]='\0'; return 0;) },
    { "next_track", nullptr, get_next_track },
    { "track_index", nullptr, APPLET_INLINE_R(snprintf(value, max_len, "%d", track_index); return 0;) },
    { "track_name", nullptr,
               APPLET_INLINE_R(strncpy(value, audio_track_get_name( track_index ), max_len); return 0;) },
    { "wifi_index", APPLET_INLINE_W( wifi_index = strtoul(value, nullptr, 10); return 0; ),
               APPLET_INLINE_R(snprintf(value, max_len, "%d", wifi_index); return 0;) },
    { "ssid",  APPLET_INLINE_W( return strcmp(value, "") ? app_wifi_set_sta_ssid_psk(wifi_index, value, nullptr): 0; ),
               APPLET_INLINE_R( strncpy(value, app_wifi_get_sta_ssid(wifi_index), max_len); return 0;) },
    { "psk",   APPLET_INLINE_W( return strcmp(value, "********") ? app_wifi_set_sta_ssid_psk(wifi_index, nullptr, value) : 0; ),
               nullptr },
    { "user",  nullptr,
               APPLET_INLINE_R( strncpy(value, settings.get_user(), max_len); return 0;) },
    { "apply_wifi", APPLET_INLINE_W( if (!strcmp(value,"true")) send_delayed_app_event( EVT_APPLY_WIFI, 0, 20000 ); return 0; ),
               nullptr },
    { "date",  APPLET_INLINE_W( if (strcmp(value, "")) update_date_time( value, nullptr ); return 0; ),
               get_date_config_value },
    { "time",  APPLET_INLINE_W( if (strcmp(value, "")) update_date_time( nullptr, value ); return 0; ),
               get_time_config_value },
    { "datetime", nullptr,
               APPLET_INLINE_R(time_t t = time( NULL ); strncpy(value, ctime( &t ), max_len); return 0;) },
    { "timezone", set_timezone_config_value,
               APPLET_INLINE_R(char *tz = getenv("TZ"); strncpy(value, tz ? tz : "None", max_len); return 0;) },
    { "color", APPLET_INLINE_W(uint32_t new_color = strtoul(&value[1], nullptr, 16);
                               leds.set_color( new_color );
                               settings.set_color( new_color );
                               return 0;),
               APPLET_INLINE_R(snprintf(value, max_len, "#%06X", settings.get_color()); return 0;) },
    { "color_mode", APPLET_INLINE_W(uint8_t new_mode = strtoul(value, nullptr, 10);
                               leds.set_mode( static_cast<LedsMode>( new_mode ) );
                               settings.set_color_mode( new_mode );
                               return 0;),
               APPLET_INLINE_R(snprintf(value, max_len, "%d", settings.get_color_mode()); return 0;) },
    { "red", nullptr,
               APPLET_INLINE_R(snprintf(value, max_len, "%d", (settings.get_color()>>16) & 0xFF); return 0;) },
    { "green", nullptr,
               APPLET_INLINE_R(snprintf(value, max_len, "%d", (settings.get_color()>>8) & 0xFF); return 0;) },
    { "blue", nullptr,
               APPLET_INLINE_R(snprintf(value, max_len, "%d", (settings.get_color()>>0) & 0xFF); return 0;) },
    { "day_br", APPLET_INLINE_W(uint8_t brightness = strtoul(value, nullptr, 10);
                                settings.set_day_brightness( brightness );
                                display.set_brightness( brightness );
                                leds.set_brightness( brightness );
                                return 0;),
               APPLET_INLINE_R(snprintf(value, max_len, "%d", settings.get_day_brightness()); return 0;) },
    { "night_br", APPLET_INLINE_W(uint8_t brightness = strtoul(value, nullptr, 10);
                                  settings.set_night_brightness( brightness );
                                  display.set_brightness( brightness );
                                  leds.set_brightness( brightness );
                                  return 0;),
               APPLET_INLINE_R(snprintf(value, max_len, "%d", settings.get_night_brightness()); return 0;) },
    { "night_mode", APPLET_INLINE_W(bool mode = (!strcmp(value, "on")) ? true: false;
                                    settings.set_night_mode( mode );
                                    return 0;),
               APPLET_INLINE_R(snprintf(value, max_len, "%s", settings.get_night_mode() ? "checked": ""); return 0;) },
    { "time_auto", set_timeauto_config_value,
               APPLET_INLINE_R(snprintf(value, max_len, "%s", settings.get_time_auto() ? "checked": ""); return 0;) },
    { "br_auto",APPLET_INLINE_W(bool mode = (!strcmp(value, "on")) ? true: false;
                                settings.set_brightness_auto( mode );
                                apply_settings();
                                return 0; ),
               APPLET_INLINE_R(snprintf(value, max_len, "%s", settings.get_brightness_auto() ? "checked": ""); return 0;) },
    { "night_time", APPLET_INLINE_W(struct tm tm_info;
                                    web_time_to_local( value, &tm_info );
                                    settings.set_night_time( &tm_info );
                                    return 0;),
               APPLET_INLINE_R(struct tm tm_info = settings.get_night_time();
                               local_time_to_web( value, max_len, &tm_info );
                               return 0;) },
    { "day_time", APPLET_INLINE_W(struct tm tm_info;
                                  web_time_to_local( value, &tm_info );
                                  settings.set_day_time( &tm_info );
                                  return 0;),
               APPLET_INLINE_R(struct tm tm_info = settings.get_day_time();
                               local_time_to_web( value, max_len, &tm_info );
                               return 0;) },
    { "ver", nullptr,
               APPLET_INLINE_R(strncpy( value, FW_VERSION, max_len ); value[max_len-1] = '\0'; return 0;) },
    { "serial", nullptr,
               APPLET_INLINE_R(strncpy( value, settings.factory().get_serial_number(), max_len );
                               value[max_len-1] = '\0';
                               return 0;) },
    { "play",  APPLET_INLINE_W( int index = strtol(value, nullptr, 10); audio_track_play( index ); return 0; ),
               nullptr },
    { "brightness", APPLET_INLINE_W(uint8_t brightness = strtoul(value, nullptr, 10);
                                    display.set_brightness( brightness );
                                    return 0; ),
               nullptr },
    { "adc",   APPLET_INLINE_W( ESP_LOGI( TAG, "adc=%d", als.get_raw_avg() ); return 0; ),
               nullptr },
    { "mqtt",  APPLET_INLINE_W( settings.set_mqtt( value );
                                send_app_event( EVT_UPDATE_MQTT, 0 );
                                return 0; ),
               APPLET_INLINE_R( snprintf(value, max_len, "%s", settings.get_mqtt()); return 0;) },
    { "temp", nullptr,
               APPLET_INLINE_R( snprintf(value, max_len, "%.2f", nixie_get_temperature()); return 0;) },
    { "reboot",APPLET_INLINE_W( send_delayed_app_event( EVT_APP_STOP, 0, 2000 ); return 0; ),
               nullptr },
    { "apply", APPLET_INLINE_W( if (!strcmp(value,"true")) save_settings(); return 0; ),
               nullptr },
    { "button", send_button_event, nullptr },
    { nullptr, APPLET_INLINE_W( return set_unknown_value(name, value, len) ),
               APPLET_INLINE_R( return get_unknown_value(name, value, max_len) ) },
    { nullptr, nullptr, nullptr },
};

const char *settings_get_tz()
{
    return settings.get_tz();
}

const char *settings_get_pass()
{
    return settings.get_pass();
}

const char *settings_get_user()
{
    return settings.get_user();
}

void settings_set_pass(const char *pass)
{
    settings.set_pass( pass );
}

void settings_set_user(const char *user)
{
    settings.set_user( user );
}

int load_settings()
{
    app_wifi_load_settings();
    settings.load();
    send_app_event( EVT_UPDATE_MQTT, 0 );
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

#define MIN_ADC_BR  138
#define MID_ADC_BR  320
#define MAX_ADC_BR  1023
#define MIN_BR      (settings.get_night_brightness())
#define MAX_BR      (settings.get_day_brightness())

int apply_settings()
{
    uint8_t brightness;
    uint8_t led_brightness;
    if ( settings.get_brightness_auto() )
    {
        int als_data = als.get_raw_avg();
        if ( als_data >= 0 )
        {
            if ( als_data <= MID_ADC_BR )
            {
                brightness = MIN_BR + (MAX_BR - MIN_BR) / 2 *
                    std::max(als_data - MIN_ADC_BR, 0) / (MID_ADC_BR - MIN_ADC_BR);
            }
            else
            {
                brightness = MIN_BR + (MAX_BR - MIN_BR) / 2 + (MAX_BR - MIN_BR) / 2 *
                    std::max(als_data - MID_ADC_BR, 0) / (MAX_ADC_BR - MID_ADC_BR);
            }
            if (brightness < MIN_BR) brightness = MIN_BR;
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
    led_brightness = brightness;
    display.set_brightness( brightness );
    leds.set_brightness( led_brightness );
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

