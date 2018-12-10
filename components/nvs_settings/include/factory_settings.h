#pragma once

#include <stdint.h>
#include <nvs_flash.h>

class FactorySettings
{
public:
    FactorySettings(const char* partition, const char* name): m_partition(partition), m_name(name) {}
    ~FactorySettings() = default;

    bool load();
    const char* get_serial_number() const;
    int get_revision() const;
    const char* get_device_number() const;

protected:
    virtual void load_product_data(nvs_handle handle) {};

private:
    const char* m_partition;
    const char* m_name;
    char m_sn[16] = {};
};
