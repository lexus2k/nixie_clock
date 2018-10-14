#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void app_wifi_init(void);
void app_wifi_done(void);
void app_wifi_start(void);
void app_wifi_stop(void);
int app_wifi_set_sta_config(const char *ssid, const char *psk);

#ifdef __cplusplus
}
#endif
