#include "state_engine.h"
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

NixieClock::NixieClock()
{
    add_state( m_init );
    add_state( m_main );
    add_state( m_show_ip );
}

bool NixieClock::on_event(SEventData event)
{
    if ( event.event == EVT_WIFI_CONNECTED )
    {
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
        return true;
    }
    if ( event.event == EVT_WIFI_DISCONNECTED )
    {
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
        leds.set_color( settings.get_color() );
        return true;
    }
    if ( event.event == EVT_WIFI_AP_MODE )
    {
        leds.set_color(2, 64, 64, 0);
        leds.set_color(3, 64, 64, 0);
        return true;
    }
    if ( event.event == EVT_UPGRADE_STATUS )
    {
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
    if ( event.event == EVT_BUTTON_PRESS && event.arg == 3 )
    {
         switch_state( CLOCK_STATE_SHOW_IP );
         return true;
    }
    return false;
}

