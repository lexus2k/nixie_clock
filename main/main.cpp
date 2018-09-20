#include "wire.h"
#include "spibus.h"
#include "hv5812.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "soc/io_mux_reg.h"

//#include "driver/i2c.h"

static uint8_t s_i2c_addr = 0b1101000; //0b1100000; // 0b1100000


// Turn the LEDs on or off. LEDs is a 16-bit int corresponding to OUT0 (LSB) to OUT15 (MSB)
void setLEDs(int LEDs)
{
    int registerVal=0;
    int registerIncrement = 0b11;
    // Write the value to the LEDs
    wire_start(s_i2c_addr, 1);
    // Write to consecutive registers, starting with LEDOUT0
    wire_send(0x80 + 0x14);
    // Write the value for LEDs
    for (int i=0; i< 16; i++)
    {
        if (LEDs & 0x01)
            registerVal += registerIncrement;
        // Move to the next LED
        LEDs >>= 1;
        // Are 4 LED values in the register now?
        if (registerIncrement == 0b11000000)
        {
            // The register can be written out now
            wire_send(registerVal);
            registerVal = 0;
            registerIncrement = 0b11;
        }
        else
        {
            // Move to the next increment
            registerIncrement <<= 2;
        }
    }
    wire_stop();
}

// Set the brightness from 0 to 0xFF
void setBrightness(int brightness)
{
    wire_start(s_i2c_addr, 1);
    // Write to the GRPPWM register
    wire_send(0x12);
    wire_send(brightness);
    wire_stop();
}

void setLedBrightness(int led, int brightness)
{
    wire_start(s_i2c_addr, 1);
    // Write to the GRPPWM register
    wire_send(0x02 + led);
    wire_send(brightness);
    wire_stop();
}

WireSPI SPI;
Hv5812 hv5812(SPI);

extern "C" void app_main()
{
    vTaskDelay(100 / portTICK_PERIOD_MS);

    gpio_iomux_out(GPIO_NUM_12, FUNC_MTDI_GPIO12, false);
//    gpio_iomux_out(GPIO_NUM_18, FUNC_GPIO18_VSPICLK, false);
//    gpio_iomux_out(GPIO_NUM_23, FUNC_GPIO23_VSPID, false);

    gpio_iomux_out(GPIO_NUM_34, FUNC_GPIO34_GPIO34, false);
    gpio_iomux_out(GPIO_NUM_17, FUNC_GPIO17_GPIO17, false);
    gpio_set_direction(GPIO_NUM_17, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_17, 0);

    gpio_iomux_out(GPIO_NUM_35, FUNC_GPIO35_GPIO35, false);

    gpio_set_direction(GPIO_NUM_12, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_12, 1);

    gpio_set_direction(GPIO_NUM_14, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_14, 0);

    gpio_set_direction(GPIO_NUM_27, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_27, 0);

    gpio_set_direction(GPIO_NUM_33, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_33, 0);

    gpio_set_direction(GPIO_NUM_32, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_32, 0);

    gpio_set_direction(GPIO_NUM_35, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_35, 0);

    uint8_t numbers[] = { 0xFF, 0xFF, 0b11110111 };
    SPI.begin();
    hv5812.begin();
    while (1)
    {
        printf("SPI working!\n");
        hv5812.write(numbers, sizeof(numbers));
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    hv5812.end();

    printf("I2C Init Waiting!\n");
    wire_init(-1);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("I2C Init Working!\n");

    wire_start(s_i2c_addr, 1);
    wire_send(0x80); // autoincrement
    wire_send(0x01);
    wire_send(0x00);
    for(int j=0; j<16; j++)
    {
        wire_send(0xFF);
    }
    wire_send(0xFF); // group
    wire_send(0x0); // not blinking
    for(int j=0; j<4; j++)
    {
        wire_send(0x0);
    }
    wire_stop();
    vTaskDelay(100 / portTICK_PERIOD_MS);

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    int r = 0b01001001;
    for(int j=0; j<3; j++)
    {
        printf("I2C!\n");
        setLEDs( r );
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        setLedBrightness(0, 64);
        setLedBrightness(1, 64);
        setLedBrightness(2, 64);
        r <<= 1;
    }

/*    for(int j=0; j<10; j++)
    {
    printf("I2C!\n");
    wire_start(s_i2c_addr);
    wire_send(0x14);
    wire_send(0b01010101);
    wire_stop();
    wire_start(s_i2c_addr);
    wire_send(0x15);
    wire_send(0b01010101);
    wire_stop();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    }*/


    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    for (int i = 2; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
