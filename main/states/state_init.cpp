#include "state_init.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_events.h"
#include "clock_states.h"
#include "clock_time.h"
#include "http_server_task.h"

#include "esp_timer.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/apps/sntp.h"
#include "mdns.h"

static uint32_t start_us;

static const char* TAG = "EVENT";

static esp_err_t start_mdns_service(void)
{
    //initialize mDNS service
    esp_err_t err = mdns_init();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to init mDNS");
        return err;
    }
    mdns_hostname_set("clock");
    mdns_instance_name_set("Nixie ESP32 clock");
    mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
    return ESP_OK;
}

static void stop_mdns_service(void)
{
    mdns_service_remove_all();
    mdns_free();
}

static int main_events_hook( uint8_t event_id, uint8_t arg )
{
    if ( event_id == EVT_WIFI_CONNECTED )
    {
//        start_tftp();
        start_webserver();
        start_mdns_service();
        if ( arg == 0 )
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
    }
    if ( event_id == EVT_WIFI_DISCONNECTED )
    {
        stop_mdns_service();
        stop_webserver();
//        stop_tftp();
        if ( arg == 0 )
        {
            wifi_is_up = false;
            sntp_stop();
        }
    }
    if ( event_id == EVT_WIFI_FAILED )
    {
        leds.set_color( settings.get_color() );
    }
    if ( event_id == EVT_WIFI_AP_MODE )
    {
        leds.set_color(2, 64, 64, 0);
        leds.set_color(3, 64, 64, 0);
    }
    if ( event_id == EVT_UPGRADE_STATUS )
    {
        switch ( arg )
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
    }
    if ( event_id == EVT_BUTTON_PRESS && arg == 3 )
    {
         states.switch_state( CLOCK_STATE_SHOW_IP );
    }
    return 0;
}

void state_init_on_enter(void)
{
    char s[16];
    states.set_event_hook( main_events_hook );
    display.set(get_time_str(s,sizeof(s)));
    apply_settings();
    display.on();
    start_us = (uint64_t)esp_timer_get_time();
}

void state_init_main(void)
{
    uint32_t us = (uint64_t)esp_timer_get_time();
    if ( us - start_us > 2000000 )
    {
         states.switch_state( CLOCK_STATE_MAIN );
    }
}

int state_init_on_event(uint8_t event, uint8_t arg)
{
    return 0;
}

void state_init_on_exit(void)
{
}

