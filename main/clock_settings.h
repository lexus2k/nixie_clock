
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
    void set_tz(const char *value);
private:
    bool m_modified;
    char m_tz[32];
};

extern "C" {
#endif

const char *settings_get_tz();

#ifdef __cplusplus
}
#endif