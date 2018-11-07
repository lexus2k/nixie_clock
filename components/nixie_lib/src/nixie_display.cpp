#include "nixie_tube.h"
#include "nixie_display.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DEBUG

NixieTubeAnimated& NixieDisplay::operator [](int index)
{
    NixieTubeAnimated* tube = get_by_index(index);
    if (tube == nullptr)
    {
        tube = &m_fake_tube;
    }
    return *tube;
}

void NixieDisplay::do_for_each(const std::function<void(NixieTubeAnimated &tube)> &func)
{
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        func( *get_by_index(i) );
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

void NixieDisplay::update()
{
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        get_by_index(i)->update();
    }
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

