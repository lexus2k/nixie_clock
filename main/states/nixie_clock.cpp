#include "nixie_clock.h"
#include "utils.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_buttons.h"
#include "clock_time.h"
#include "clock_states.h"
#include "clock_events.h"

#include "state_main.h"
#include "state_hw_init.h"
#include "state_init.h"
#include "state_show_ip.h"
#include "state_show_temp.h"
#include "state_sleep.h"


#include "http_server_task.h"
#include "wifi_task.h"

#include "esp_timer.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/apps/sntp.h"
#include "mdns.h"
#include "version.h"

#include "http_ota_upgrade.h"

#include <esp_system.h>

static const char* TAG = "EVENT";

NixieClock::NixieClock()
{
    add_state<StateHwInit>();
    add_state<StateInit>();
    add_state<StateMain>();
    add_state<StateShowIp>();
    add_state<StateShowTemp>();
    add_state<StateSleep>();
}

EEventResult NixieClock::on_event(SEventData event)
{
    if ( event.event == EVT_WIFI_CONNECTED )
    {
        ESP_LOGI(TAG, "EVENT: WIFI CONNECTED");
//        start_tftp();
        start_webserver();
        start_mdns_service();
        if ( event.arg == EVT_ARG_STA )
        {
            wifi_sta_is_up = true;
            sntp_setoperatingmode(SNTP_OPMODE_POLL);
            sntp_setservername(0, (char*)"pool.ntp.org");
            if ( settings.get_time_auto() )
            {
                ESP_LOGI(TAG, "Initializing SNTP");
                sntp_init();
            }
            leds.set_color( settings.get_color() );
            send_event( SEventData{ EVT_CHECK_FW, 0} );
        }
        else if ( event.arg == EVT_ARG_AP )
        {
            ESP_LOGI(TAG, "EVENT: WIFI AP MODE");
            leds.set_color(2, 64, 64, 0);
            leds.set_color(3, 64, 64, 0);
        }
        return EEventResult::PROCESSED_AND_HOOKED;
    }
    if ( event.event == EVT_WIFI_DISCONNECTED )
    {
        ESP_LOGI(TAG, "EVENT: WIFI DISCONNECTED");
        stop_mdns_service();
        stop_webserver();
//        stop_tftp();
        if ( event.arg == EVT_ARG_STA )
        {
            leds.set_color(2, 0, 0, 128);
            leds.set_color(3, 0, 0, 128);
            wifi_sta_is_up = false;
            sntp_stop();
        }
        return EEventResult::PROCESSED_AND_HOOKED;
    }
    if ( event.event == EVT_UPGRADE_STATUS )
    {
        switch ( event.arg )
        {
            case EVT_UPGRADE_STARTED:
                ESP_LOGI( TAG, "EVENT: UPGRADE started" );
                leds.set_color(0, 0, 48);
                leds.enable_blink();
                break;
            case EVT_UPGRADE_SUCCESS:
                ESP_LOGI( TAG, "EVENT: UPGRADE successful" );
                leds.set_color( 0, 128, 0 );
                display.off();
                display.update();
                break;
            case EVT_UPGRADE_FAILED:
                ESP_LOGI( TAG, "EVENT: UPGRADE failed" );
                leds.set_color(128, 0, 0);
                send_delayed_event( SEventData{ EVT_APP_UPDATE_COLOR , 0}, 15000 );
                break;
            default: break;
        }
        return EEventResult::PROCESSED_AND_HOOKED;
    }
    if ( event.event == EVT_APP_UPDATE_COLOR )
    {
        apply_settings();
        return EEventResult::PROCESSED_AND_HOOKED;
    }
    if ( event.event == EVT_APP_STOP )
    {
        stop();
        return EEventResult::PROCESSED_AND_HOOKED;
    }
    if ( event.event == EVT_APPLY_WIFI )
    {
        app_wifi_apply_sta_settings();
        return EEventResult::PROCESSED_AND_HOOKED;
    }
    if ( event.event == EVT_CHECK_FW )
    {
        if ( wifi_sta_is_up )
        {
            http_client_ota_upgrade( "https://github.com/lexus2k/nixie_clock/raw/master/binaries/nixie_clock.txt",
                                     "https://github.com/lexus2k/nixie_clock/raw/master/binaries/nixie_clock.bin",
                                     on_validate_version,
                                     on_upgrade_start,
                                     on_upgrade_end );
            send_delayed_event( SEventData{ EVT_CHECK_FW, 0}, 24*3600000 );
        }
        return EEventResult::PROCESSED_AND_HOOKED;
    }
    if ( event.event == EVT_BUTTON_PRESS && event.arg == EVT_BUTTON_4 )
    {
        push_state( CLOCK_STATE_SHOW_IP );
        return EEventResult::PROCESSED_AND_HOOKED;
    }
    if ( event.event == EVT_BUTTON_LONG_HOLD && event.arg == EVT_BUTTON_4 )
    {
        leds.set_color(2, 0, 0, 128);
        leds.set_color(3, 0, 0, 128);
        app_wifi_start_ap_only();
        return EEventResult::PROCESSED_AND_HOOKED;
    }
    return EEventResult::NOT_PROCESSED;
}

void NixieClock::on_update()
{
    audio_player.update();
    display.update();
//    display.print();
    abuttons.update();
    dbuttons.update();
    als.update();
    static int counter = 0;
    counter++;
    if (counter++ > 1023)
    {
        static uint32_t min_heap_old = UINT32_MAX;
        counter = 0;
        uint32_t min_heap = esp_get_minimum_free_heap_size();
        if ( min_heap < min_heap_old )
        {
            min_heap_old = min_heap;
            ESP_LOGI( TAG, "MIN HEAP: %d, CURRENT HEAP: %d",
                           esp_get_minimum_free_heap_size(),
                           esp_get_free_heap_size() );
        }
    }
    // Too many false positive cases
/*    if ( als.is_peak_detected(50, 200) )
    {
        als.reset_peak_detector();
        send_event( {EVT_BUTTON_PRESS, EVT_BUTTON_1} );
    } */
}

bool NixieClock::on_begin()
{
    settings.load_factory(); // MUST BE CALLED BEFORE NVS_FLASH_INIT()

    ESP_LOGI(TAG, "CURRENT FW: %s", FW_VERSION);

    gpio_iomux_out(GPIO_NUM_4, FUNC_GPIO4_GPIO4, false);
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_INPUT);
    // Enable flash boot button, to run http update server on press
    gpio_iomux_out(GPIO_NUM_0, FUNC_GPIO0_GPIO0, false);
    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT);
    gpio_pullup_en(GPIO_NUM_0);

    printf("Serial number: %s\n", settings.factory().get_serial_number());
    // Init NVS used by the components
    nvs_flash_init();
    settings.load();
    load_hardware_configuration();

    setenv("TZ", settings_get_tz(), 1); // https://www.systutorials.com/docs/linux/man/3-tzset/
    tzset();

    // init led controllers
    leds.set_min_pwm(2, 2, 2);
    leds.set_max_pwm(232,175,112);

    abuttons.onButtonDown(on_abutton_down);
    abuttons.onButtonUp(on_abutton_up);
    abuttons.onButtonHold(on_abutton_hold);
    dbuttons.onButtonDown(on_dbutton_down);
    dbuttons.onButtonUp(on_dbutton_up);
    dbuttons.onButtonHold(on_dbutton_hold);

    // Init i2c and spi interfaces first
    SPI.begin();
    I2C.begin();
    // Leds must be started before RTC chip, because TLC59116
    // uses ALLCALADR=1101000, the same as DS3232 (1101000)
    leds.begin();
    // init display: disable all anod pins
    display.begin();
    rtc_chip.begin();
    abuttons.begin();
    dbuttons.begin();
    als.begin();
    temperature.begin();
    audio_player.set_prebuffering( 32 );
    audio_player.begin( EAudioChannels::RIGHT_ONLY );

    leds.enable();
    leds.set_color(0, 64, 0);
    leds.set_color(2, 0, 0, 128);
    leds.set_color(3, 0, 0, 128);
    app_wifi_init();
    if (rtc_chip.is_available())
    {
        update_date_time_from_rtc();
    }
    app_wifi_start();
    return true;
}

void NixieClock::on_end()
{
    app_wifi_done();
    audio_player.end();
    temperature.end();
    als.end();
    abuttons.end();
    dbuttons.end();
    display.end();
    I2C.end();
    SPI.end();
    settings.end();
    nvs_flash_deinit();
}
