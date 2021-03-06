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
    bool set(const char* key, bool value);
    bool get(const char* key, bool &value);
    bool set(const char* key, uint8_t value);
    bool get(const char* key, uint8_t &value);
    bool set(const char* key, uint32_t value);
    bool get(const char* key, uint32_t &value);
    bool set(const char *key, char *value, size_t max_size);
    bool get(const char *key, char *value, size_t max_size);

private:
    const char* m_name;
    nvs_handle m_handle = 0;
    nvs_open_mode m_open_mode;
};
