#pragma once

#include "wire_i2c.h"
#include "spibus.h"
#include "nixie_display.h"
#include "tiny_buttons.h"
#include "tiny_digital_buttons.h"
#include "clock_settings.h"
#include "audio_player.h"
#include "nixie_ds3232.h"
#include "gl5528.h"
#include "lm35dz.h"
#include "controllers/led_controller.h"

class CustomNixieDisplay;

extern WireI2C I2C;
extern WireSPI SPI;
extern LedController leds;
extern Ds3231 rtc_chip;
extern CustomNixieDisplay display;
extern AudioPlayer audio_player;
extern TinyAnalogButtons abuttons;
extern TinyDigitalButtons dbuttons;
extern ClockSettings settings;
extern Gl5528 als;
extern Lm35Dz temperature;


