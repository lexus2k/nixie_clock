/*
    This file is part of Nixie Clock program.
    Copyright (C) 2019  Alexey Dynda

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
static Tlc59116Leds hw_leds(I2C);
LedController leds(hw_leds);
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
    if ( !nixie_clock.begin( CLOCK_STATE_HW_INIT ) )
    {
        leds.set_status( LedStatus::BOOT_FAILED );
        nixie_clock.end();
        fprintf( stderr, "Failed to start device\n" );
        for(;;)
        {
            vTaskDelay(200);
        }
    }

    nixie_clock.loop( 20 );
    nixie_clock.end();
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
