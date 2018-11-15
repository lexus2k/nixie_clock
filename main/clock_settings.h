
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

    const char *get_tz();
    uint32_t get_color();
    void set_tz(const char *value);
    void set_color(uint32_t value);
private:
    bool m_modified;
    char m_tz[32];
    uint32_t m_color;
};

extern "C" {
#endif

const char *settings_get_tz();

#ifdef __cplusplus
}
#endif