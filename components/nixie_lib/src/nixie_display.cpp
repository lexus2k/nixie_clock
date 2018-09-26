#include "nixie_tube.h"
#include "nixie_display.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void NixieDisplay::set_pin_muxer(PinMux* muxer)
{
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        get_by_index(i)->set_pin_muxer( muxer );
    }
}

void NixieDisplay::set_anods(gpio_num_t *pins)
{
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        get_by_index(i)->set_anod(pins[i]);
    }
}

void NixieDisplay::begin()
{
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        get_by_index(i)->begin();
    }
}

void NixieDisplay::end()
{
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        get_by_index(i)->end();
    }
}

