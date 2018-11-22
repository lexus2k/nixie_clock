#include "nvs_settings.h"

bool NvsSettings::begin(nvs_open_mode open_mode)
{
    bool result = false;
    m_open_mode = open_mode;
    esp_err_t ret = nvs_open(m_name,
                             open_mode,
                             &m_handle);
    if (ret == ESP_ERR_NVS_NOT_FOUND)
    {
        m_handle = 0;
    }
    else if (ret != ESP_OK)
    {
        m_handle = 0;
    }
    else
    {
        result = true;
    }
    return result;
}

void NvsSettings::end()
{
    if (m_handle)
    {
        if ( m_open_mode == NVS_READWRITE )
        {
            nvs_commit(m_handle);
        }
        nvs_close(m_handle);
        m_handle = 0;
    }
}

bool NvsSettings::erase()
{
    bool result = false;
    if (m_handle)
    {
        if ( m_open_mode == NVS_READWRITE )
        {
            if ( nvs_erase_all(m_handle) == ESP_OK )
            {
                result = true;
            }
        }
    }
    return result;
}

bool NvsSettings::set(const char* key, bool value)
{
    uint8_t v = value ? 1: 0;
    return set(key, v);
}

bool NvsSettings::get(const char* key, bool &value)
{
    uint8_t v;
    bool result = get(key, v);
    value = v ? true: false;
    return result;
}

bool NvsSettings::set(const char* key, uint8_t value)
{
    if ( nvs_set_u8(m_handle, key, value) != ESP_OK )
    {
        return false;
    }
    return true;
}

bool NvsSettings::get(const char* key, uint8_t &value)
{
    if ( nvs_get_u8(m_handle, key, &value) != ESP_OK )
    {
        return false;
    }
    return true;
}

bool NvsSettings::set(const char* key, uint32_t value)
{
    if ( nvs_set_u32(m_handle, key, value) != ESP_OK )
    {
        return false;
    }
    return true;
}

bool NvsSettings::get(const char* key, uint32_t &value)
{
    if ( nvs_get_u32(m_handle, key, &value) != ESP_OK )
    {
        return false;
    }
    return true;
}

bool NvsSettings::set(const char *key, char *value, size_t max_size)
{
    if ( nvs_set_blob(m_handle, key, value, max_size) != ESP_OK )
    {
        return false;
    }
    return true;
}

bool NvsSettings::get(const char *key, char *value, size_t max_size)
{
    if ( nvs_get_blob(m_handle, key, value, &max_size) != ESP_OK )
    {
        return false;
    }
    return true;
}

/*
bool NvsSettings::set_var(const char* key, const char* type, const char* value)
{
static const char* TYPE_STRING = "s";
static const char* TYPE_U8 = "u8";
static const char* TYPE_U16 = "u16";
    if (*type == 0)
    {
        char* endptr;
        uint16_t uval = strtoul(value, &endptr, 10);
    }
}
*/