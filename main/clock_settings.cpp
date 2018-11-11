#include "clock_settings.h"
#include "clock_hardware.h"

ClockSettings::ClockSettings()
    : NvsSettings("nixie")
    , m_modified(false)
    , m_tz("VLAT-10:00:00")
{
}

void ClockSettings::save()
{
    if (m_modified)
    {
        begin(NVS_READWRITE);
        set("tz", m_tz, sizeof(m_tz));
        end();
    }
    m_modified = false;
}

void ClockSettings::load()
{
    begin(NVS_READONLY);
    get("tz", m_tz, sizeof(m_tz));
    end();
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

const char *settings_get_tz()
{
    return settings.get_tz();
}
