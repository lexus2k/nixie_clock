#include "wire.h"
#include "pin_muxers.h"
#include "nixie_display.h"
#include "spibus.h"
#include "hv5812.h"
#include "tlc59116.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "soc/io_mux_reg.h"
#include "driver/ledc.h"

//#include "driver/i2c.h"

/*
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

void enableLED()
{
    ledc_timer_config_t ledc_timer{};
    ledc_timer.duty_resolution = LEDC_TIMER_12_BIT; // resolution of PWM duty
    ledc_timer.freq_hz = 500;                       // frequency of PWM signal
    ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;   // timer mode
    ledc_timer.timer_num = LEDC_TIMER_0;             // timer index

    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel[1]{};
    ledc_channel[0].channel    = LEDC_CHANNEL_0;
    ledc_channel[0].duty       = 0;
    ledc_channel[0].gpio_num   = GPIO_NUM_12;
    ledc_channel[0].speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel[0].timer_sel  = LEDC_TIMER_0;
    ledc_channel_config(&ledc_channel[0]);
    ledc_set_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel, 512);
    ledc_update_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel);
}
*/

#define MAX_PINS_PER_TUBE 12

gpio_num_t g_anods[] =
{
    GPIO_NUM_12,
    GPIO_NUM_14,
    GPIO_NUM_27,
    GPIO_NUM_33,
    GPIO_NUM_32,
    GPIO_NUM_35,
};

uint8_t g_tube_pin_map[] =
{
  // 1   2   3   4   5   6   7   8   9   0  ,    ,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
    12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
    36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
};

WireI2C I2C;
WireSPI SPI;
Tlc59116 left_leds(I2C, 0b1100000);
Tlc59116 right_leds(I2C, 0b1100000);
Hv5812 hv5812(SPI);
PinMuxHv5812 pin_muxer(SPI, 4);
NixieDisplay6IN14 display;

void app_init()
{
    gpio_iomux_out(GPIO_NUM_12, FUNC_MTDI_GPIO12, false);

    gpio_iomux_out(GPIO_NUM_34, FUNC_GPIO34_GPIO34, false);
    gpio_iomux_out(GPIO_NUM_17, FUNC_GPIO17_GPIO17, false);

    gpio_iomux_out(GPIO_NUM_35, FUNC_GPIO35_GPIO35, false);

    pin_muxer.set_map(g_tube_pin_map, sizeof(g_tube_pin_map), MAX_PINS_PER_TUBE);
    display.set_pin_muxer( &pin_muxer );
    display.set_anods(g_anods);

    SPI.begin();
    I2C.begin();
//    hv5812.begin();
    pin_muxer.begin();
    display.begin();
    left_leds.begin();
    right_leds.begin();
    vTaskDelay(100 / portTICK_PERIOD_MS);

    display[0].initLedcTimer();
    display[0].set_brightness(32);
    display[0].on();
    left_leds.enable_leds(0b010010010);
    left_leds.set_brightness(32);

    pin_muxer.update();
}

void app_done()
{
    pin_muxer.end();
    display.end();
    I2C.end();
    SPI.end();
}


extern "C" void app_main()
{
    vTaskDelay(100 / portTICK_PERIOD_MS);
    app_init();

    while(1) {};

    app_done();

//    gpio_set_direction(GPIO_NUM_17, GPIO_MODE_OUTPUT);
//    gpio_set_level(GPIO_NUM_17, 0);

//    enableLED();
//    gpio_set_direction(GPIO_NUM_12, GPIO_MODE_OUTPUT);
//    gpio_set_level(GPIO_NUM_12, 1);

//    gpio_set_direction(GPIO_NUM_14, GPIO_MODE_OUTPUT);
//    gpio_set_level(GPIO_NUM_14, 0);

//    gpio_set_direction(GPIO_NUM_27, GPIO_MODE_OUTPUT);
//    gpio_set_level(GPIO_NUM_27, 0);

//    gpio_set_direction(GPIO_NUM_33, GPIO_MODE_OUTPUT);
//    gpio_set_level(GPIO_NUM_33, 0);

//    gpio_set_direction(GPIO_NUM_32, GPIO_MODE_OUTPUT);
//    gpio_set_level(GPIO_NUM_32, 0);

//    gpio_set_direction(GPIO_NUM_35, GPIO_MODE_OUTPUT);
//    gpio_set_level(GPIO_NUM_35, 0);

/*    uint8_t numbers[] = { 0xFF, 0xFF, 0b11110111 };
    while (1)
    {
        printf("SPI working!\n");
        hv5812.write(numbers, sizeof(numbers));
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    hv5812.end();*/

/*    printf("I2C Init Waiting!\n");
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
    vTaskDelay(100 / portTICK_PERIOD_MS);*/

/*    vTaskDelay(1000 / portTICK_PERIOD_MS);

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
    }*/

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
/*    esp_chip_info_t chip_info;
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
    printf("Restarting now.\n"); */
    fflush(stdout);
    esp_restart();
}
