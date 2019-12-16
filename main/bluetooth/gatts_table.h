#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Attributes State Machine */
enum
{
    IDX_SVC,
    IDX_SSID_DECL,
    IDX_SSID_VAL,
    IDX_SSID_DESC,
    IDX_SSID_FMT,
//    IDX_CHAR_CFG_A,

    IDX_CHAR_B,
    IDX_CHAR_VAL_B,

    IDX_CHAR_C,
    IDX_CHAR_VAL_C,

    HRS_IDX_NB,
};

void clock_start_ble_service(void);

void clock_stop_ble_service(void);

#ifdef __cplusplus
}
#endif
