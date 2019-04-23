#include "nixie_clock.h"
#include "utils.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_buttons.h"
#include "clock_time.h"
#include "clock_states.h"
#include "clock_events.h"

#include "http_server_task.h"
#include "wifi_task.h"

#include "esp_timer.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/apps/sntp.h"
#include "mdns.h"
#include "version.h"

#include "http_ota_upgrade.h"

static const char* TAG = "EVENT";

NixieClock::NixieClock()
{
    add_state( m_hw_init );
    add_state( m_init );
    add_state( m_main );
    add_state( m_show_ip );
    add_state( m_show_temp );
    add_state( m_sleep );
}

bool NixieClock::on_event(SEventData event)
{
    if ( event.event == EVT_WIFI_CONNECTED )
    {
        ESP_LOGI(TAG, "EVENT: WIFI CONNECTED");
//        start_tftp();
        start_webserver();
        start_mdns_service();
        if ( event.arg == 0 )
        {
            wifi_is_up = true;
            sntp_setoperatingmode(SNTP_OPMODE_POLL);
            sntp_setservername(0, (char*)"pool.ntp.org");
            if ( settings.get_time_auto() )
            {
                ESP_LOGI(TAG, "Initializing SNTP");
                sntp_init();
            }
            leds.set_color( settings.get_color() );
        }
        http_client_ota_upgrade( "https://github.com/lexus2k/nixie_clock/raw/master/binaries/nixie_clock.txt",
                                 "https://github.com/lexus2k/nixie_clock/raw/master/binaries/nixie_clock.bin",
                                 on_validate_version,
                                 on_upgrade_start,
                                 on_upgrade_end );
        return true;
    }
    if ( event.event == EVT_WIFI_DISCONNECTED )
    {
        ESP_LOGI(TAG, "EVENT: WIFI DISCONNECTED");
        stop_mdns_service();
        stop_webserver();
//        stop_tftp();
        if ( event.arg == 0 )
        {
            wifi_is_up = false;
            sntp_stop();
        }
        return true;
    }
    if ( event.event == EVT_WIFI_FAILED )
    {
        ESP_LOGI(TAG, "EVENT: WIFI FAILED");
        leds.set_color( settings.get_color() );
        return true;
    }
    if ( event.event == EVT_WIFI_AP_MODE )
    {
        ESP_LOGI(TAG, "EVENT: WIFI AP MODE");
        leds.set_color(2, 64, 64, 0);
        leds.set_color(3, 64, 64, 0);
        return true;
    }
    if ( event.event == EVT_UPGRADE_STATUS )
    {
        ESP_LOGI(TAG, "EVENT: UPGRADE: %X", event.arg);
        switch ( event.arg )
        {
            case EVT_UPGRADE_STARTED:
                leds.set_color(0, 0, 48);
                leds.enable_blink();
                break;
            case EVT_UPGRADE_SUCCESS:
                leds.set_color( 0x007F00 );
                display.off();
                display.update();
                break;
            case EVT_UPGRADE_FAILED:
                leds.set_color(48, 0, 0);
                break;
            default: break;
        }
        return true;
    }
    if ( event.event == EVT_BUTTON_PRESS && event.arg == EVT_BUTTON_4 )
    {
        push_state( CLOCK_STATE_SHOW_IP );
        return true;
    }
    if ( event.event == EVT_BUTTON_LONG_HOLD && event.arg == EVT_BUTTON_4 )
    {
         app_wifi_start_ap_only();
         return true;
    }
    return false;
}

void NixieClock::on_update()
{
    audio_player.update();
    display.update();
//    display.print();
    abuttons.update();
    dbuttons.update();
    als.update();
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
    audio_player.set_prebuffering( 30 );
    audio_player.begin();

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
