#pragma once

#include "esp_wifi.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_MANAGER_CONFIG_INIT()  { \
            .on_connect = NULL, \
            .on_disconnect = NULL, \
        }

typedef struct
{
    void (*on_connect)(bool isAp);
    void (*on_disconnect)(bool isAp);
} wifi_manager_config_t;

void wifi_manager_init(wifi_manager_config_t *conf);
void wifi_manager_deinit(void);
void wifi_manager_reload_settings(void);
void wifi_manager_clear_settings(void);
//void wifi_manager_save_settings(void);

bool wifi_manager_start_ap(wifi_config_t *ap);
void wifi_manager_generate_ap_config( wifi_config_t *ap_config,
                                      const char *prefix,
                                      char *password );

int wifi_manager_add_network(wifi_config_t *sta);
bool wifi_manager_modify_network(int index, wifi_config_t *conf);
bool wifi_manager_get_status(wifi_config_t *conf);
bool wifi_manager_connect(int index);
bool wifi_manager_get_network(int index, wifi_config_t *conf);
int wifi_manager_network_count(void);

void wifi_manager_disconnect(void);

#ifdef __cplusplus
}
#endif
