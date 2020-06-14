#include "nixie_clock.h"
#include "utils.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_buttons.h"
#include "clock_time.h"
#include "states/clock_states.h"
#include "clock_events.h"
#include "bluetooth/gatts_table.h"
#include "platform/system.h"
#include "controllers/mqtt_controller.h"

#include "states/state_main.h"
#include "states/state_hw_init.h"
#include "states/state_init.h"
#include "states/state_show_ip.h"
#include "states/state_show_temp.h"
#include "states/state_sleep.h"
#include "states/state_time_setup.h"
#include "states/state_alarm_setup.h"
#include "states/state_alarm.h"
#include "esp_ota_ops.h"

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
    SM_STATE( StateHwInit,        CLOCK_STATE_HW_INIT      );
    SM_STATE( StateInit,          CLOCK_STATE_APP_INIT     );
    SM_STATE( StateMain,          CLOCK_STATE_MAIN         );
    SM_STATE( StateShowIp,        CLOCK_STATE_SHOW_IP      );
    SM_STATE( StateShowTemp,      CLOCK_STATE_SHOW_TEMP    );
    SM_STATE( StateSleep,         CLOCK_STATE_SLEEP        );
    SM_STATE( StateTimeSetup,     CLOCK_STATE_SETUP_TIME   );
    SM_STATE( StateAlarmSetup,    CLOCK_STATE_SETUP_ALARM  );
    SM_STATE( StateAlarm,         CLOCK_STATE_ALARM        );

//    CLOCK_STATE_SETUP_ALARM,
}

STransitionData NixieClock::onEvent(SEventData event)
{
    StateUid sid = getActiveId();
    // ********************** This is global transition table actual for all states *****************************************
    //             event id              event arg         transition_func          type        to state
    NO_TRANSITION( EVT_APP_STOP,         SM_EVENT_ARG_ANY, stop() )
    NO_TRANSITION( EVT_APPLY_WIFI,       SM_EVENT_ARG_ANY, app_wifi_apply_sta_settings() )
    NO_TRANSITION( EVT_COMMIT_UPGRADE,   SM_EVENT_ARG_ANY, esp_ota_mark_app_valid_cancel_rollback() )
    NO_TRANSITION( EVT_WIFI_CONNECTED,   SM_EVENT_ARG_ANY, on_wifi_connected( event.arg == EVT_ARG_STA ) )
    NO_TRANSITION( EVT_WIFI_DISCONNECTED,SM_EVENT_ARG_ANY, on_wifi_disconnected( event.arg == EVT_ARG_STA ) )
    NO_TRANSITION( EVT_UPGRADE_STATUS,   SM_EVENT_ARG_ANY, on_upgrade_status( event.arg ) )
    NO_TRANSITION( EVT_UPDATE_MQTT,      SM_EVENT_ARG_ANY, m_mqtt.end(); if ( wifi_sta_is_up ) m_mqtt.begin( settings.get_mqtt() ) )
    NO_TRANSITION( EVT_CHECK_FW,         SM_EVENT_ARG_ANY, on_check_new_fw() )
    FROM_STATE( CLOCK_STATE_MAIN )
    {
        NO_TRANSITION( EVT_BUTTON_LONG_HOLD, EVT_BUTTON_4,  leds.set_status( LedStatus::AP_STARTED ); app_wifi_start_ap_only() )
    }
    TRANSITION_TBL_END
}

void NixieClock::on_wifi_connected(bool staMode)
{
    ESP_LOGI(TAG, "EVENT: WIFI CONNECTED");
    start_webserver();
    start_mdns_service();
    if ( staMode )
    {
        wifi_sta_is_up = true;
        m_mqtt.begin( settings.get_mqtt() );
//"mqtt://mqtt:mqtt@192.168.1.101");
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, (char*)"pool.ntp.org");
        if ( settings.get_time_auto() )
        {
            ESP_LOGI(TAG, "Initializing SNTP");
            sntp_init();
        }
        leds.set_status( LedStatus::NORMAL );
        sendEvent( SEventData{ EVT_CHECK_FW, 0} );
    }
    else
    {
        ESP_LOGI(TAG, "EVENT: WIFI AP MODE");
        leds.set_status( LedStatus::AP_CLIENT_CONNECTED );
//            clock_start_ble_service();
    }
}

void NixieClock::on_wifi_disconnected(bool staMode)
{
    ESP_LOGI(TAG, "EVENT: WIFI DISCONNECTED");
    stop_webserver();
    stop_mdns_service();
    if ( staMode )
    {
        leds.set_status( LedStatus::STA_DISCONNECTED );
        wifi_sta_is_up = false;
        m_mqtt.end();
        sntp_stop();
    }
    else
    {
        leds.set_status( LedStatus::AP_STARTED );
//            clock_stop_ble_service();
    }
}

void NixieClock::on_upgrade_status( uintptr_t status )
{
    switch ( status )
    {
        case EVT_UPGRADE_STARTED:
            ESP_LOGI( TAG, "EVENT: UPGRADE started" );
            leds.set_status( LedStatus::UPGRADE_IN_PROGRESS );
            break;
        case EVT_UPGRADE_SUCCESS:
            ESP_LOGI( TAG, "EVENT: UPGRADE successful" );
            leds.set_status( LedStatus::UPGRADE_SUCCESSFUL );
            display.off();
            display.update();
            break;
        case EVT_UPGRADE_FAILED:
            ESP_LOGI( TAG, "EVENT: UPGRADE failed" );
            leds.set_status( LedStatus::UPGRADE_FAILED );
            break;
        default: break;
    }
}

void NixieClock::on_check_new_fw()
{
    if ( wifi_sta_is_up )
    {
        http_client_ota_upgrade( "https://github.com/lexus2k/nixie_clock/raw/master/binaries/nixie_clock.txt",
                                 "https://github.com/lexus2k/nixie_clock/raw/master/binaries/nixie_clock.bin",
                                 on_validate_version,
                                 on_upgrade_start,
                                 on_upgrade_end );
        sendEvent( SEventData{ EVT_CHECK_FW, 0}, 24*3600000 );
    }
}

void NixieClock::onUpdate()
{
    m_mqtt.update();
    audio_player.update();
    display.update();
//    display.print();
    abuttons.update();
    dbuttons.update();
    leds.update();
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

bool NixieClock::onBegin()
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
    // after startup, TLC59116 is reconfigured to avoid using of 1101000 i2c address
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

    leds.on();
    leds.set_status( LedStatus::BOOTING );
    leds.set_color( static_cast<LedsMode>(settings.get_color_mode()), settings.get_color() );
    app_wifi_init();
    if (rtc_chip.is_available())
    {
        update_date_time_from_rtc();
    }
    app_wifi_start();

    esp_ota_img_states_t ota_state = ESP_OTA_IMG_VALID;
    esp_ota_get_state_partition( esp_ota_get_running_partition(), &ota_state );
    if ( ota_state == ESP_OTA_IMG_PENDING_VERIFY )
    {
        sendEvent( SEventData{ EVT_COMMIT_UPGRADE, 0}, 60000 );
    }

    return true;
}

void NixieClock::onEnd()
{
    app_wifi_done();
    audio_player.end();
    temperature.end();
    als.end();
    abuttons.end();
    dbuttons.end();
    display.end();
    leds.end();
    I2C.end();
    SPI.end();
    settings.end();
    nvs_flash_deinit();
}

uint64_t NixieClock::getMicros()
{
    return micros64();
}
