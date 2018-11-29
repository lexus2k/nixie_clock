#pragma once

#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wire_i2c_intf.h"

enum
{
    TLC59116_MODE_GROUP_DIMMING   = (0<<5),
    TLC59116_MODE_GROUP_BLINKING  = (1<<5),
};

class Tlc59116
{
public:
    Tlc59116(IWireI2C& i2c, uint8_t address);
    ~Tlc59116();

    bool begin();
    void end();

    /**
     * Enables specified led outputs.
     * each bit corresponds to led output to be enabled
     */
    void on( uint16_t leds );

    /**
     * Disables specified led outputs.
     * each bit corresponds to led output to be disabled
     */
    void off( uint16_t leds );

    /**
     * Disables all led outputs
     */
    void off();

    /**
     * enables led outputs
     */
    void set( uint16_t leds );

    /**
     * Sets group brightness
     */
    void set_brightness(uint8_t br);

    /**
     * Sets channel brightness
     */
    void set_brightness(uint8_t led, uint8_t brightness);

    void set_blinking(uint8_t blinking);

    void set_mode(uint8_t mode);

private:
    IWireI2C& m_i2c;
    uint8_t m_address;
    uint16_t m_enabled_leds = 0;
    bool m_detected = false;

    void update_leds(uint16_t leds);
};

