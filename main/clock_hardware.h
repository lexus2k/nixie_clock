#pragma once

#include "wire.h"
#include "spibus.h"
#include "nixie_display.h"
#include "tlc59116.h"
#include "tiny_buttons.h"
#include "nvs_settings.h"
#include "audio_player.h"
#include "nixie_ds3232.h"

extern WireI2C I2C;
extern WireSPI SPI;
extern Tlc59116 left_leds;
extern Tlc59116 right_leds;
extern Ds3231 rtc_chip;
extern NixieDisplay6IN14 display;
extern AudioPlayer audio_player;
extern TinyAnalogButtons buttons;
extern NvsSettings settings;

