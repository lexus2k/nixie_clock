#pragma once

#include <stdint.h>

void wire_init(int8_t busId);

void wire_start(uint8_t addr, int write_mode);

void wire_stop(void);

void wire_send(uint8_t data);

void wire_close(void);
