#include "factory_settings.h"


bool FactorySettings::load()
{
    nvs_handle handle;
    nvs_flash_init_partition(m_partition);
    if ( nvs_open_from_partition(m_partition, m_name, NVS_READONLY, &handle) != ESP_OK )
    {
        return false;
    }
    size_t s = sizeof(m_sn);
    nvs_get_str(handle, "serial_number", m_sn, &s);
    load_product_data(handle);
    nvs_close(handle);
    nvs_flash_deinit_partition(m_partition);
    return true;
}

const char* FactorySettings::get_serial_number() const
{
    return m_sn;
}

int FactorySettings::get_revision() const
{
    return m_sn[5] - '0';
}

const char* FactorySettings::get_device_number() const
{
    return &m_sn[6];
}
