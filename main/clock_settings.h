
#pragma once

#ifdef __cplusplus
#include "nvs_settings.h"

class ClockSettings: public NvsSettings
{
public:
    ClockSettings();
    ~ClockSettings() = default;

    void save();
    void load();

    void set_tz(const char *value);
    const char *get_tz();

    void set_color(uint32_t value);
    uint32_t get_color();

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
private:
    bool m_modified;
    char m_tz[32];
    uint32_t m_color;
    bool m_night_mode;
    uint8_t m_day_brightness;
    uint8_t m_night_brightness;
    uint32_t m_day_time;
    uint32_t m_night_time;
    bool m_time_auto;
};

extern bool wifi_is_up;

extern "C" {
#endif
const char *settings_get_tz();
int get_config_value(const char *param, char *data, int max_len);
int try_config_value(const char *param, char *data, int max_len);
int apply_new_wifi_config(char *buffer, int len);
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
