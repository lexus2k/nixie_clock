
#pragma once

#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

char *get_time_str(char *buf, int size);

void update_date_time(const char *new_date, const char *new_time);
void update_date_ex(uint16_t year, uint8_t month, uint8_t day);
void update_time_ex(uint8_t hour, uint8_t min, uint8_t sec);
void update_rtc_chip(bool manual);
void update_date_time_from_rtc();
void web_time_to_local(const char *str, struct tm *tm_info);
char *local_time_to_web(char *str, int max_len, const struct tm *tm_info);
uint32_t pack_time_u32(const struct tm *tm_info);
void unpack_time_u32(struct tm *tm_info, uint32_t packed_time);
void get_current_time(struct tm *tm_info);
void set_current_time(struct tm *tm_info);
uint16_t daytime_to_minutes(struct tm *tm_info);

#ifdef __cplusplus
}
#endif
