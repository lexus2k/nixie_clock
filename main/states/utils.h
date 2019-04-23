#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

bool on_validate_version(const char * new_ver);

void on_upgrade_start(void);

void on_upgrade_end(bool success);

bool start_mdns_service(void);

void stop_mdns_service(void);

void load_hardware_configuration(void);

#ifdef __cplusplus
}
#endif
