#include "states/clock_states.h"

#include "wire.h"
#include "pin_muxers.h"
#include "nixie_display.h"
#include "spibus.h"
#include "hv5812.h"
#include "tlc59116.h"
#include "tiny_buttons.h"
#include "nvs_settings.h"
#include "audio_player.h"
#include "nixie_melodies.h"
#include "wifi_task.h"
#include "nixie_ds3232.h"
#include "clock_hardware.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "soc/io_mux_reg.h"
#include "driver/ledc.h"
#include "driver/adc.h"

#define MAX_PINS_PER_TUBE 12

#ifdef REV_1

gpio_num_t g_anods[] =
{
    GPIO_NUM_12,
    GPIO_NUM_14,
    GPIO_NUM_27,
    GPIO_NUM_33,
    GPIO_NUM_32,
    GPIO_NUM_16,
};

ledc_channel_t pwm_channels[] =
{
    LEDC_CHANNEL_0,
    LEDC_CHANNEL_1,
    LEDC_CHANNEL_2,
    LEDC_CHANNEL_3,
    LEDC_CHANNEL_4,
    LEDC_CHANNEL_6,
};

uint8_t g_tube_pin_map[] =
{
  // 0   1   2   3   4   5   6   7   8   9  ,    ,
     4, 19, 18, 17, 16, 15,  0,  1,  2,  3, 71, 70,
    10,  5,  6,  7,  8,  9, 14, 13, 12, 11, 69, 68,
    24, 39, 38, 37, 36, 35, 20, 21, 22, 23, 67, 66,
    30, 25, 26, 27, 28, 29, 34, 33, 32, 31, 65, 64,
    44, 59, 58, 57, 56, 55, 40, 41, 42, 43, 63, 62,
    50, 45, 46, 47, 48, 49, 54, 53, 52, 51, 61, 60,
};

int16_t g_buttons_map[] =
{
    640,
    479,
    298,
};

#endif

WireI2C I2C;
WireSPI SPI;
Tlc59116 left_leds(I2C, 0b1100000);
Tlc59116 right_leds(I2C, 0b1100001);
Ds3231 rtc_chip(I2C);
PinMuxHv5812 pin_muxer(SPI, GPIO_NUM_17, 4);
NixieDisplay6IN14 display;
AudioPlayer audio_player;
TinyAnalogButtons buttons(ADC1_CHANNEL_0, g_buttons_map, sizeof(g_buttons_map) / sizeof(g_buttons_map[0]));
NvsSettings settings("clock");
SmEngine states(clock_states);

static void app_init()
{
    // Init NVS used by the components
    nvs_flash_init();

    gpio_iomux_out(GPIO_NUM_12, FUNC_MTDI_GPIO12, false);
    gpio_iomux_out(GPIO_NUM_34, FUNC_GPIO34_GPIO34, false);
    gpio_iomux_out(GPIO_NUM_17, FUNC_GPIO17_GPIO17, false);
    gpio_iomux_out(GPIO_NUM_35, FUNC_GPIO35_GPIO35, false);

    pin_muxer.set_map(g_tube_pin_map, sizeof(g_tube_pin_map), MAX_PINS_PER_TUBE);
    display.set_pin_muxer( &pin_muxer );
    display.set_anods(g_anods);

    // Init ledc timer: TODO: to make as part of display initialization
    display.enable_pwm( pwm_channels );
    display[0].initLedcTimer();
    display[0].enable_pwm(LEDC_CHANNEL_0);

    // Init i2c and spi interfaces first
    SPI.begin();
    I2C.begin();
    // init display: disable all anod pins
    display.begin();
    // init led controllers
    left_leds.begin();
    right_leds.begin();
    rtc_chip.begin();
    buttons.begin();
    audio_player.begin();
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Enable flash boot button, to run http update server on press
    gpio_iomux_out(GPIO_NUM_0, FUNC_GPIO0_GPIO0, false);
    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT);
    gpio_pullup_en(GPIO_NUM_0);

    app_wifi_init();
    rtc_chip.getDateTime();
}

static void app_run_test()
{
    // Tubes test
    // turn on green leds
    left_leds.enable_leds(0b010010010);
    left_leds.set_brightness(32);

/*
    // send changes to hardware
//    display.update();

//    display.set_brightness(32);
//    display.on();

    for(int i=0; i<10;i++)
    {
        char s[7]{};
        memset(s, '0' + i, 6);
        display.set(s);
        display.update();
        vTaskDelay(300 / portTICK_PERIOD_MS);
    } */

//    audio_player.play( &melodyMonkeyIslandP );
}

static void app_run()
{
    app_run_test();
    app_wifi_start();
    // init display: disable all anod pins
    display.begin();
    states.switch_state( CLOCK_STATE_INIT );

    for(;;)
    {
        if (gpio_get_level(GPIO_NUM_0) == 0)
        {
            app_wifi_done();
        }
        vTaskDelay(25 / portTICK_PERIOD_MS);
        audio_player.update();
        display.update();
        buttons.update();
        states.update();
//        int val = buttons.getButtonId();
//        printf("BUTTON:%i\n", val);
    }
}

static void app_done()
{
    app_wifi_done();

    audio_player.end();
    buttons.end();
    display.end();
    pin_muxer.end();
    I2C.end();
    SPI.end();
    settings.end();
    nvs_flash_deinit();
}

static void main_task(void *pvParameter)
{
    app_init();

    app_run();

    app_done();

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

extern "C" void app_main()
{
    xTaskCreate(&main_task, "main_task", 4096, NULL, 5, NULL);
}
