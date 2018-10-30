#include "clock_buttons.h"
#include "clock_events.h"
#include "clock_hardware.h"
#include "states/clock_states.h"

#include "esp_log.h"

// TODO: Remove
#include "nixie_melodies.h"

static bool long_hold_sent;

static const char TAG[] = "BTN";

void on_abutton_down(uint8_t id, uint16_t timeDeltaMs)
{
    // TODO: Put sound click on press
    long_hold_sent = false;
    ESP_LOGI( TAG, "%d is down", id );
}

void on_abutton_up(uint8_t id, uint16_t timeDeltaMs)
{
    if ( timeDeltaMs < 400 ) send_app_event( EVT_BUTTON_PRESS, id);
    ESP_LOGI( TAG, "%d is up", id );
}

void on_abutton_hold(uint8_t id, uint16_t timeDeltaMs)
{
    if ((timeDeltaMs > 1000) && (!long_hold_sent))
    {
        send_app_event( EVT_BUTTON_LONG_HOLD, id );
        ESP_LOGI( TAG, "%d long hold", id );
        abuttons.disableUpAction();
        long_hold_sent = true;
    }
}

void on_dbutton_down(uint8_t id, uint16_t timeDeltaMs)
{
    // TODO: Put sound click on press
    long_hold_sent = false;
    ESP_LOGI( TAG, "%d is down", id + ANALOG_BUTTONS_COUNT );
}

void on_dbutton_up(uint8_t id, uint16_t timeDeltaMs)
{
    if ( timeDeltaMs < 400 ) send_app_event( EVT_BUTTON_PRESS, id + ANALOG_BUTTONS_COUNT);
    ESP_LOGI( TAG, "%d is up", id + ANALOG_BUTTONS_COUNT );
}

void on_dbutton_hold(uint8_t id, uint16_t timeDeltaMs)
{
    if ((timeDeltaMs > 1000) && (!long_hold_sent))
    {
        send_app_event( EVT_BUTTON_LONG_HOLD, id + ANALOG_BUTTONS_COUNT );
        ESP_LOGI( TAG, "%d long hold", id + ANALOG_BUTTONS_COUNT );
        dbuttons.disableUpAction(id);
        long_hold_sent = true;
    }
}

extern const uint8_t test_vgz_start[] asm("_binary_test_vgz_start");
extern const uint8_t test_vgz_end[]   asm("_binary_test_vgz_end");

void buttons_start_audio(void)
{
//    audio_player.play( &melodyMonkeyIslandP );
    audio_player.playVGM( test_vgz_start, test_vgz_end - test_vgz_start );
}