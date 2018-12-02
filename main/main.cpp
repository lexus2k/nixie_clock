#include "states/clock_states.h"
#include "clock_display.h"
#include "clock_time.h"
#include "states/state_engine.h"

#include "wire_i2c.h"
#include "pin_muxers.h"
#include "nixie_display.h"
#include "spibus.h"
#include "hv5812.h"
#include "tlc59116.h"
#include "nvs_settings.h"
#include "audio_player.h"
#include "nixie_melodies.h"
#include "wifi_task.h"
#include "nixie_ds3232.h"
#include "clock_hardware.h"
#include "clock_buttons.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "soc/io_mux_reg.h"
#include "driver/ledc.h"
#include "driver/adc.h"
#include "esp_task_wdt.h"

// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

#define MAX_PINS_PER_TUBE 12

#ifdef REV_1

int16_t g_buttons_map[ANALOG_BUTTONS_COUNT] =
{
    640,
    479,
    298,
};

button_desc_t g_dbuttons_map[] =
{
    { GPIO_NUM_0, 0 },
};

#endif

WireI2C I2C( I2C_NUM_1, 400000 );
WireSPI SPI;
Tlc59116Leds leds(I2C);
Ds3231 rtc_chip(I2C);
CustomNixieDisplay display;
AudioPlayer audio_player;
TinyAnalogButtons abuttons(ADC1_CHANNEL_0, g_buttons_map, sizeof(g_buttons_map) / sizeof(g_buttons_map[0]));
TinyDigitalButtons dbuttons(g_dbuttons_map, sizeof(g_dbuttons_map) / sizeof(g_dbuttons_map[0]));
ClockSettings settings;
NixieClock nixie_clock;

static void app_init()
{
    gpio_iomux_out(GPIO_NUM_4, FUNC_GPIO4_GPIO4, false);
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_INPUT);
    if (gpio_get_level(GPIO_NUM_4) == 0)
    {
        printf("Main board revision 1 detected\n");
    }
    else
    {
        printf("Main board revision 2 detected\n");
    }
    // Init NVS used by the components
    nvs_flash_init();
    settings.load();
    setenv("TZ", settings_get_tz(), 1); // https://www.systutorials.com/docs/linux/man/3-tzset/
    tzset();

    gpio_iomux_out(GPIO_NUM_12, FUNC_MTDI_GPIO12, false);
    gpio_iomux_out(GPIO_NUM_34, FUNC_GPIO34_GPIO34, false);
    gpio_iomux_out(GPIO_NUM_17, FUNC_GPIO17_GPIO17, false);
    gpio_iomux_out(GPIO_NUM_35, FUNC_GPIO35_GPIO35, false);

    // Init i2c and spi interfaces first
    SPI.begin();
    I2C.begin();
    // init led controllers
    leds.set_min_pwm(2, 2, 2);
    leds.set_max_pwm(232,175,112);
    // Leds must be started before RTC chip, because TLC59116
    // uses ALLCALADR=1101000, the same as DS3232 (1101000)
    leds.begin();
    // init display: disable all anod pins
    display.begin();
    rtc_chip.begin();
    abuttons.onButtonDown(on_abutton_down);
    abuttons.onButtonUp(on_abutton_up);
    abuttons.onButtonHold(on_abutton_hold);
    abuttons.begin();
    dbuttons.onButtonDown(on_dbutton_down);
    dbuttons.onButtonUp(on_dbutton_up);
    dbuttons.onButtonHold(on_dbutton_hold);
    dbuttons.begin();
    audio_player.begin();
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Enable flash boot button, to run http update server on press
    gpio_iomux_out(GPIO_NUM_0, FUNC_GPIO0_GPIO0, false);
    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT);
    gpio_pullup_en(GPIO_NUM_0);

    leds.enable();
    leds.set_color(0, 0, 64, 0);
    leds.set_color(1, 0, 64, 0);
    leds.set_color(2, 0, 0, 64);
    leds.set_color(3, 0, 0, 64);
    leds.set_color(4, 0, 64, 0);
    leds.set_color(5, 0, 64, 0);
    app_wifi_init();
    if (rtc_chip.is_available())
    {
        update_date_time_from_rtc();
    }
    if ( !nixie_clock.begin() )
    {
        nixie_clock.end();
        fprintf( stderr, "Failed to start device\n" );
        for(;;)
           vTaskDelay(200);
    }
}

static void app_run()
{
    app_wifi_start();
    nixie_clock.switch_state( CLOCK_STATE_INIT );

    for(;;)
    {
//        esp_task_wdt_reset();
        vTaskDelay(25 / portTICK_PERIOD_MS);
        audio_player.update();
        display.update();
        abuttons.update();
        dbuttons.update();
        nixie_clock.update();
    }
}

static void app_done()
{
    nixie_clock.end();
    app_wifi_done();

    audio_player.end();
    abuttons.end();
    dbuttons.end();
    display.end();
    I2C.end();
    SPI.end();
    settings.end();
    nvs_flash_deinit();
}

static void main_task(void *pvParameter)
{
    app_init();

//    buttons_start_audio();
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
    xTaskCreate(&main_task, "main_task", 4096 /* 4096 */, NULL, 5, NULL);
    for(;;)
    {
        esp_task_wdt_reset();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
