#include "clock_time.h"
#include "clock_hardware.h"

#include "lwip/apps/sntp.h"
#include "esp_log.h"

#include <sys/time.h>
#include <time.h>

static const char *TAG = "TIME";

char *get_time_str(char *buf, int size, struct tm *info)
{
    struct tm tm_info;
    if (info)
    {
        tm_info = *info;
    }
    else
    {
        get_current_time(&tm_info);
    }
    snprintf( buf, size, CLOCK_TIME_FORMAT_STRING,
        tm_info.tm_hour/10, tm_info.tm_hour%10, (tm_info.tm_sec & 1) ? '.': ' ',
        tm_info.tm_min/10, tm_info.tm_min%10, (tm_info.tm_sec & 1) ? '.': ' ',
        tm_info.tm_sec/10, tm_info.tm_sec%10 );
    return buf;
}

char *get_date_str(char *buf, int size, struct tm *info)
{
    struct tm tm_info;
    if (info)
    {
        tm_info = *info;
    }
    else
    {
        get_current_time(&tm_info);
    }
    snprintf( buf, size, CLOCK_DATE_FORMAT_STRING,
        tm_info.tm_mday / 10, tm_info.tm_mday % 10,
        (tm_info.tm_mon + 1) / 10, (tm_info.tm_mon + 1) % 10,
        ((tm_info.tm_year / 10) % 10), tm_info.tm_year % 10 );
    return buf;
}

void update_date_time(const char *new_date, const char *new_time)
{
    if ( (new_date == nullptr) && (new_time == nullptr) ) return;
    struct tm tm_info;
    get_current_time(&tm_info);
    if ( new_date != nullptr )
    {
        tm_info.tm_year = strtoul(new_date, nullptr, 10) - 1900;
        tm_info.tm_mon = strtoul(new_date+5, nullptr, 10);
        tm_info.tm_mday = strtoul(new_date+8, nullptr, 10);
    }
    if ( new_time != nullptr )
    {
        web_time_to_local(new_time, &tm_info);
    }
    set_current_time( &tm_info );
    ESP_LOGI( TAG, "Setting time/date to %s", asctime( &tm_info ));
}

void update_date_ex(uint16_t year, uint8_t month, uint8_t day)
{
    char s[16];
    snprintf(s, sizeof(s), "%04d-%02d-%02d", year, month, day);
    update_date_time( s, nullptr );
}

void update_time_ex(uint8_t hour, uint8_t min, uint8_t sec)
{
    char s[16];
    snprintf(s, sizeof(s), "%02d-%02d-%02d", hour, min, sec);
    update_date_time( nullptr, s );
}

void update_rtc_chip(bool manual)
{
    if (rtc_chip.is_available() && (sntp_enabled() || manual))
    {
        struct tm tm_info;
        get_current_time(&tm_info);
        rtc_chip.m_hours = rtc_chip.toInternal(tm_info.tm_hour);
        rtc_chip.m_minutes = rtc_chip.toInternal(tm_info.tm_min);
        rtc_chip.m_seconds = rtc_chip.toInternal(tm_info.tm_sec);
        rtc_chip.m_year = rtc_chip.toInternal(tm_info.tm_year - 100);
        rtc_chip.m_month = rtc_chip.toInternal(tm_info.tm_mon);
        rtc_chip.m_day = rtc_chip.toInternal(tm_info.tm_mday);
        rtc_chip.setDateTime();
        ESP_LOGI( TAG, "Updating rtc time/date to %s", asctime( &tm_info ));
    }
}

void update_date_time_from_rtc()
{
    if (rtc_chip.is_available())
    {
        rtc_chip.getDateTime();
        struct tm tm_info{};
        tm_info.tm_hour = rtc_chip.toDecimal(rtc_chip.m_hours);
        tm_info.tm_min = rtc_chip.toDecimal(rtc_chip.m_minutes);
        tm_info.tm_sec = rtc_chip.toDecimal(rtc_chip.m_seconds);
        tm_info.tm_year = rtc_chip.toDecimal(rtc_chip.m_year) + 100;
        tm_info.tm_mon = rtc_chip.toDecimal(rtc_chip.m_month);
        tm_info.tm_mday = rtc_chip.toDecimal(rtc_chip.m_day);
        tm_info.tm_isdst = -1;
        time_t t = mktime(&tm_info);
        struct timeval tv{};
        tv.tv_sec = t;
        settimeofday( &tv, nullptr );
        ESP_LOGI( TAG, "Setting time/date to %s from RTC", asctime( &tm_info ));
    }
}

void web_time_to_local(const char *str, struct tm *tm_info)
{
    tm_info->tm_hour = strtoul(str, nullptr, 10);
    tm_info->tm_min = strtoul(str+3, nullptr, 10);
    tm_info->tm_sec = 0;
}

char *local_time_to_web(char *str, int max_len, const struct tm *tm_info)
{
    if (!tm_info) return nullptr;
    strftime(str, max_len, "%H:%M:%S", tm_info);
    return str;
}

uint32_t pack_time_u32(const struct tm *tm_info)
{
    if (!tm_info) return 0;
    return (tm_info->tm_hour << 16) | (tm_info->tm_min << 8) | (tm_info->tm_sec);
}

void unpack_time_u32(struct tm *tm_info, uint32_t packed_time)
{
    if (!tm_info) return;
    tm_info->tm_hour = (packed_time >> 16) & 0xFF;
    tm_info->tm_min = (packed_time >> 8) & 0xFF;
    tm_info->tm_sec = (packed_time >> 0) & 0xFF;
}

void get_current_time(struct tm *tm_info)
{
    struct timeval tv;
    if ( gettimeofday(&tv, nullptr) != 0)
    {
        return;
    }
    localtime_r(&tv.tv_sec, tm_info);
}

void set_current_time(struct tm *tm_info)
{
    struct timeval tv;
    if ( gettimeofday(&tv, nullptr) != 0)
    {
        return;
    }
    time_t t = mktime( tm_info );
    tv.tv_sec = t;
    settimeofday( &tv, nullptr );
}

uint16_t daytime_to_minutes(struct tm *tm_info)
{
    return tm_info->tm_hour * 60 + tm_info->tm_min;
}
