#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    EVT_NONE,
    EVT_WIFI_CONNECTED,
    EVT_WIFI_DISCONNECTED,
    EVT_BUTTON_PRESS,
    EVT_BUTTON_LONG_HOLD,
    EVT_UPGRADE_STATUS,
    EVT_APP_STOP,
    EVT_APPLY_WIFI,
    EVT_CHECK_FW,
    EVT_COMMIT_UPGRADE,
    EVT_UPDATE_MQTT,
};

enum
{
    EVT_ARG_AP,
    EVT_ARG_STA,
};

enum
{
    EVT_UPGRADE_STARTED,
    EVT_UPGRADE_SUCCESS,
    EVT_UPGRADE_FAILED,
};

void send_app_event(uint8_t event_id, uint8_t arg);
void send_delayed_app_event(uint8_t event_id, uint8_t arg, uint16_t delayMs);

#ifdef __cplusplus
}
#endif
