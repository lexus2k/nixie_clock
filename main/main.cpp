#include "states/clock_states.h"
#include "clock_display.h"
#include "clock_time.h"
#include "states/nixie_clock.h"
#include "ram_logger.h"

#include "wire_i2c.h"
#include "pin_muxers.h"
#include "nixie_display.h"
#include "spibus.h"
#include "hv5812.h"
#include "tlc59116.h"
#include "nvs_settings.h"
#include "wifi_task.h"
#include "nixie_ds3232.h"
#include "clock_hardware.h"
#include "clock_events.h"

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

WireI2C I2C( I2C_NUM_1, 400000 );
WireSPI SPI;
Tlc59116Leds leds(I2C);
Ds3231 rtc_chip(I2C);
CustomNixieDisplay display;
TinyAnalogButtons abuttons;
TinyDigitalButtons dbuttons;
ClockSettings settings;
NixieClock nixie_clock;
Gl5528 als;
Lm35Dz temperature;


static void main_task(void *pvParameter)
{
    ram_logger_init(3072);
    if ( !nixie_clock.begin() )
    {
        leds.set_color(192, 64, 64);
        leds.enable_blink();
        nixie_clock.end();
        fprintf( stderr, "Failed to start device\n" );
        for(;;)
        {
            vTaskDelay(200);
        }
    }

    nixie_clock.switch_state( CLOCK_STATE_HW_INIT );
    nixie_clock.loop( 20 );
//        esp_task_wdt_reset();
    nixie_clock.end();

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
    ram_logger_free();
    esp_restart();
}

extern "C" void app_main()
{
    xTaskCreatePinnedToCore(&main_task, "main_task", 4096, NULL, 5, NULL, 1);
    for(;;)
    {
        esp_task_wdt_reset();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
