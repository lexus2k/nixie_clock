#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    EVT_WIFI_CONNECTED,
    EVT_WIFI_AP_MODE,
    EVT_WIFI_DISCONNECTED,
    EVT_WIFI_FAILED,
    EVT_BUTTON_PRESS,
    EVT_BUTTON_LONG_HOLD,
    EVT_UPGRADE_STATUS,
};

enum
{
    EVT_UPGRADE_STARTED,
    EVT_UPGRADE_SUCCESS,
    EVT_UPGRADE_FAILED,
};

void send_app_event(uint8_t event_id, uint8_t arg);

#ifdef __cplusplus
}
#endif
