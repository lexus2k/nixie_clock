#include "wire.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "driver/i2c.h"

static int8_t s_bus_id;

static i2c_cmd_handle_t s_cmd_handle;

void wire_init(int8_t busId)
{
    // init your interface here
    if ( busId < 0) busId = I2C_NUM_1;
    s_bus_id = busId;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = 21; // I2C_EXAMPLE_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = 22; // I2C_EXAMPLE_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000; //I2C_EXAMPLE_MASTER_FREQ_HZ;
    i2c_param_config(s_bus_id, &conf);
    i2c_driver_install(s_bus_id, conf.mode, 0, 0, 0);
//                       I2C_EXAMPLE_MASTER_RX_BUF_DISABLE,
//                       I2C_EXAMPLE_MASTER_TX_BUF_DISABLE, 0);
}

void wire_start(uint8_t addr, int write_mode)
{
    // ... Open i2c channel for your device with specific s_i2c_addr
    s_cmd_handle = i2c_cmd_link_create();
    i2c_master_start(s_cmd_handle);
    if (write_mode)
    {
        i2c_master_write_byte(s_cmd_handle, ( addr << 1 ) | I2C_MASTER_WRITE, 0x1);
    }
    else
    {
        i2c_master_write_byte(s_cmd_handle, ( addr << 1 ) | I2C_MASTER_READ, 0x1);
    }
}

void wire_stop(void)
{
    // ... Complete i2c communication
    i2c_master_stop(s_cmd_handle);
    /*esp_err_t ret =*/ i2c_master_cmd_begin(s_bus_id, s_cmd_handle, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(s_cmd_handle);
}

void wire_send(uint8_t data)
{
    // ... Send byte to i2c communication channel
    i2c_master_write_byte(s_cmd_handle, data, 0x1);
}

void wire_close(void)
{
    // ... free all i2c resources here
    i2c_driver_delete(s_bus_id);
}

/*
static void platform_i2c_send_buffer(const uint8_t *data, uint16_t len)
{
    // ... Send len bytes to i2c communication channel here
    while (len--)
    {
        platform_i2c_send(*data);
        data++;
    }
//    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
}
*/