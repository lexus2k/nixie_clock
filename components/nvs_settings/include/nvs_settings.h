#pragma once

#include <stdint.h>
#include <nvs_flash.h>

class NvsSettings
{
public:
    NvsSettings(const char* name): m_name(name) {}
    ~NvsSettings() = default;

    bool begin(nvs_open_mode open_mode = NVS_READONLY);
    void end();

    bool erase();
    bool set(const char* key, uint8_t value);
//    bool nvs_set_blob(nvs_handle handle, const char *key, const void *value, size_t length)
    bool get(const char* key, uint8_t &value);
//    bool set_var(const char* key, const char*type, const char* value);

private:
    const char* m_name;
    nvs_handle m_handle = 0;
    nvs_open_mode m_open_mode;
};
