
#pragma once

#ifdef __cplusplus
#include "nvs_settings.h"
#include "factory_settings.h"

class ClockSettings: public NvsSettings
{
public:
    ClockSettings();
    ~ClockSettings() = default;

    void load_factory();
    void save();
    void load();
// Factory settings
    const FactorySettings& factory();

// Permanent settings
    void set_tz(const char *value);
    const char *get_tz();

    void set_mqtt(const char *value);
    const char *get_mqtt();

    void set_color(uint32_t value);
    uint32_t get_color();

    void set_color_mode(uint8_t value);
    uint8_t get_color_mode();

    void set_night_mode(bool enable);
    bool get_night_mode();

    void set_time_auto(bool enable);
    bool get_time_auto();

    void set_day_brightness(uint8_t value);
    uint8_t get_day_brightness();

    void set_night_brightness(uint8_t value);
    uint8_t get_night_brightness();

    void set_day_time(struct tm *tm_info);
    struct tm get_day_time();

    void set_night_time(struct tm *tm_info);
    struct tm get_night_time();

    void set_brightness_auto(bool auto_brightness);
    bool get_brightness_auto();

// Runtime settings
    void set_highlight_enable(bool enable);
    bool get_highlight_enable();

    void set_predefined_color(int index);
    int get_predefined_color();

    void set_alarm( uint32_t value );
    uint32_t get_alarm();

private:
    FactorySettings m_factory;
    bool m_modified = false;
    // Permanent settings
    char m_tz[32];
    uint32_t m_color = 0x00007F00;
    uint8_t m_color_mode = 0;
    bool m_night_mode = false;
    uint8_t m_day_brightness = 160;
    uint8_t m_night_brightness = 64;
    uint32_t m_day_time = 0x00080000;
    uint32_t m_night_time = 0x00150000;
    bool m_time_auto = true;
    bool m_brightness_auto = false;
    // Runtime settings
    bool m_highlight_enabled = true;
    int m_predefined_color = 0;
    char m_mqtt[64]{};
    uint32_t m_alarm = 0;
};

extern bool wifi_sta_is_up;

extern "C" {
#endif

#include "http_applet_engine.h"

extern const applet_param_t config_params[];

const char *settings_get_tz();
int load_settings();
int reset_settings();
int save_settings();
int apply_settings();

/**
 * returns whever it is night time or not.
 * depends on night mode option
 */
int is_night_time();

#ifdef __cplusplus
}
#endif
