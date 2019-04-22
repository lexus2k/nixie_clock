#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void app_wifi_init(void);
void app_wifi_done(void);
void app_wifi_start(void);
void app_wifi_start_ap_only(void);
void app_wifi_stop(void);

const char *app_wifi_get_sta_ssid(void);
int app_wifi_set_sta_ssid_psk(int index, const char *ssid, const char *psk);
int app_wifi_apply_sta_settings(void);
void app_wifi_load_settings(void);

#ifdef __cplusplus
}
#endif
