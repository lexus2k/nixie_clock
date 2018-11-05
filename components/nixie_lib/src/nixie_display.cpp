#include "nixie_tube.h"
#include "nixie_display.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DEBUG

void NixieDisplay::do_for_each(const std::function<void(NixieTubeAnimated &tube)> &func)
{
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        func( *get_by_index(i) );
    }
}

void NixieDisplay::set_pin_muxer(PinMux* muxer)
{
    do_for_each( [&muxer](NixieTubeAnimated &tube)->void
    {
        tube.set_pin_muxer( muxer );
    } );
    m_pin_muxer = muxer;
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
    m_pin_muxer->begin();
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        get_by_index(i)->begin();
    }
    m_pin_muxer->update();
}

void NixieDisplay::end()
{
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        get_by_index(i)->end();
    }
}

void NixieDisplay::update()
{
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        get_by_index(i)->update();
    }
    // Send data to hardware
    m_pin_muxer->update();
}

void NixieDisplay::enable_pwm(ledc_channel_t* channel, ledc_timer_t timer)
{
    NixieTube::init_ledc_timer( timer );
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        get_by_index(i)->enable_pwm(channel[i],  timer);
    }
}

void NixieDisplay::set_pwm_range(uint16_t min_pwm, uint16_t max_pwm )
{
    NixieTube::set_pwm_range( min_pwm, max_pwm );
}

void NixieDisplay::set(const char *p)
{
#ifdef DEBUG
    static char b[20]{};
    if (strncmp(b, p, 4))
    {
        strcpy(b,p);
        fprintf(stderr, "%s\n", p);
    }
#endif
    for (int i=0; (get_by_index(i) != nullptr) && (*p != '\0') ; i++, p++ )
    {
        get_by_index(i)->set(p[0] - '0');
    }
}

void NixieDisplay::scroll(const char *p)
{
    for (int i=0; (get_by_index(i) != nullptr) && (*p != '\0') ; i++, p++ )
    {
        get_by_index(i)->scroll(p[0] - '0');
    }
}

void NixieDisplay::overlap(const char *p)
{
    for (int i=0; (get_by_index(i) != nullptr) && (*p != '\0') ; i++, p++ )
    {
        get_by_index(i)->overlap(p[0] - '0');
    }
}

void NixieDisplay::on()
{
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        get_by_index(i)->on();
    }
}

void NixieDisplay::set_brightness(uint8_t brightness)
{
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        get_by_index(i)->set_brightness(brightness);
    }
}

