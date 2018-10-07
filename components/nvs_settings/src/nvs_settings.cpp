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

