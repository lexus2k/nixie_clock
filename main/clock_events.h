#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    EVT_WIFI_CONNECTED,
    EVT_WIFI_DISCONNECTED,
    EVT_WIFI_FAILED,
};

void send_app_event(uint8_t event_id, uint8_t arg);

#ifdef __cplusplus
}
#endif
