#pragma once

#include "esp_wifi.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_MANAGER_CONFIG_INIT()  { \
            .on_connect = NULL, \
            .on_disconnect = NULL, \
            .auto_reconnect = true, \
        }


/**
 * Configuration of wifi manager
 */
typedef struct
{
    void (*on_connect)(bool isAp); ///< on_connect callback is called when client is connected to AP or STA connected to AP
    void (*on_disconnect)(bool isAp); ///< on_disconnect callback if AP client is disconnected, or STA is down
    bool auto_reconnect; ///< not used
} wifi_manager_config_t;

/**
 * Initializes wifi manager.
 * @param conf point to wifi_manager_config_t structure
 */
void wifi_manager_init(wifi_manager_config_t *conf);

/**
 * Stops wifi and deinitializes wifi manager
 */
void wifi_manager_deinit(void);

/**
 * Loads settings from NVRAM. Function checks for WiFi saved config, stored
 * by ESP32 Wifi configuration, and then checks "wifi_mgr" section for stored
 * configs in NVS ram.
 */
void wifi_manager_load_settings(void);

/**
 * Stores settings to "wifi_mgr" of NVRAM.
 */
void wifi_manager_save_settings(void);

void wifi_manager_reload_settings(void);
void wifi_manager_clear_settings(void);

/**
 * Starts AP, sta connection will be closed.
 * @param ap pointer to AP configuration
 */
bool wifi_manager_start_ap(wifi_config_t *ap);

/**
 * Generates AP configuration, based on prefix and password values
 * @param ap pointer to configuration to fill
 * @param prefix prefix to used for generating ssid. MAC address will be
 *        added as suffix to ssid name.
 * @param password password to use for AP, if NULL is passed "00000000" will
 *        be used as password.
 */
void wifi_manager_generate_ap_config( wifi_config_t *ap_config,
                                      const char *prefix,
                                      char *password );
/**
 * Adds new AP to the list of remembered Access Points
 * @param sta pointer to sta config to add
 * @return returns index of added configuration. In case of
 *         error, function returns -1
 */
int wifi_manager_add_network(wifi_config_t *sta);

/**
 * Modifies stored STA configuration. This doesn't change values stored in NVS.
 * To save changes to NVS, please use wifi_manager_save_config();
 * @param index index of sta record to modify
 * @param config pointer to sta config.
 * @return returns true if successful
 */
bool wifi_manager_modify_network(int index, wifi_config_t *conf);
bool wifi_manager_get_status(wifi_config_t *conf);
bool wifi_manager_connect(int index);
bool wifi_manager_get_network(int index, wifi_config_t *conf);
int wifi_manager_network_count(void);

void wifi_manager_disconnect(void);

#ifdef __cplusplus
}
#endif
