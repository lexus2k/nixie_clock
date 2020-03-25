#include "clock_buttons.h"
#include "clock_events.h"
#include "clock_hardware.h"
#include "states/clock_states.h"

#include "esp_log.h"

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
    ESP_LOGI( TAG, "%d is down", id + abuttons.get_count() );
}

void on_dbutton_up(uint8_t id, uint16_t timeDeltaMs)
{
    if ( timeDeltaMs < 400 ) send_app_event( EVT_BUTTON_PRESS, id + abuttons.get_count() );
    ESP_LOGI( TAG, "%d is up", id + abuttons.get_count() );
}

void on_dbutton_hold(uint8_t id, uint16_t timeDeltaMs)
{
    if ((timeDeltaMs > 1000) && (!long_hold_sent))
    {
        send_app_event( EVT_BUTTON_LONG_HOLD, id + abuttons.get_count() );
        ESP_LOGI( TAG, "%d long hold", id + abuttons.get_count() );
        dbuttons.disableUpAction(id);
        long_hold_sent = true;
    }
}

