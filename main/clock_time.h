
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

char *get_time_str(char *buf, int size);

void update_date_time(const char *new_date, const char *new_time);
void update_date_ex(uint16_t year, uint8_t month, uint8_t day);
void update_time_ex(uint8_t hour, uint8_t min, uint8_t sec);
void update_rtc_chip(bool manual);
void update_date_time_from_rtc();


#ifdef __cplusplus
}
#endif