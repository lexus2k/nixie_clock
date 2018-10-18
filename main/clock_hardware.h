#pragma once

#include "wire.h"
#include "spibus.h"
#include "nixie_display.h"
#include "tiny_buttons.h"
#include "nvs_settings.h"
#include "audio_player.h"
#include "nixie_ds3232.h"
#include "rgb_leds.h"

#define REV_1

extern WireI2C I2C;
extern WireSPI SPI;
extern Tlc59116Leds leds;
extern Ds3231 rtc_chip;
extern NixieDisplay6IN14 display;
extern AudioPlayer audio_player;
extern TinyAnalogButtons buttons;
extern NvsSettings settings;

