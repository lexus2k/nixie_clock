#include "wire.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "driver/i2c.h"
#include "esp_log.h"

static const char *TAG = "I2C";

//    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);

WireI2C::WireI2C(i2c_port_t bus, uint32_t clock)
    : m_bus(bus)
    , m_clock( clock )
{
}

WireI2C::~WireI2C()
{
}

bool WireI2C::begin()
{
    if (m_clock == 0)
    {
        m_clock = 100000;
    }
    if ( m_bus < 0 )
    {
        m_bus = I2C_NUM_1;
    }
    m_bus = m_bus;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    // TODO: Update pin numbers depending on bus number
    conf.sda_io_num = GPIO_NUM_21;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = GPIO_NUM_22;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = m_clock;
    i2c_param_config(m_bus, &conf);
    i2c_driver_install(m_bus, conf.mode, 0, 0, 0);
    return true;
}

void WireI2C::end()
{
    i2c_driver_delete(m_bus);
}

bool WireI2C::beginTransmission(uint8_t address)
{
    m_handle = i2c_cmd_link_create();
    m_address = address;
    i2c_master_start(m_handle);
    i2c_master_write_byte(m_handle, ( address << 1 ) | I2C_MASTER_WRITE, 0x1);
    return true;
}

int WireI2C::write(uint8_t data)
{
    i2c_master_write_byte(m_handle, data, 0x1);
    return 1;
}

int WireI2C::endTransmission()
{
    i2c_master_stop(m_handle);
    esp_err_t ret = i2c_master_cmd_begin(m_bus, m_handle, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(m_handle);
    if (ret == ESP_FAIL)
    {
        ESP_LOGE(TAG, "No response from 0x%02X", m_address);
    }
    return ret == ESP_OK ? 0 : -1;
}

bool WireI2C::requestFrom(uint8_t address, int len)
{
    m_handle = i2c_cmd_link_create();
    m_address = address;
    i2c_master_start(m_handle);
    i2c_master_write_byte(m_handle, ( address << 1 ) | I2C_MASTER_READ, 0x1);
    return false;
}

uint8_t WireI2C::read()
{
    uint8_t data;
    i2c_master_read_byte(m_handle, &data, I2C_MASTER_ACK);
    return data;
}
