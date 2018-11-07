#include "pin_muxers.h"
#include "clock_display.h"
#include "spibus.h"
#include "hv5812.h"
#include "clock_hardware.h"
#include "hv5812_group_controller.h"

#define TUBE_PWM_FREQ_HZ (200)

#ifdef REV_1

static gpio_num_t g_anods[] =
{
    GPIO_NUM_12,
    GPIO_NUM_14,
    GPIO_NUM_27,
    GPIO_NUM_33,
    GPIO_NUM_32,
    GPIO_NUM_16,
};

static ledc_channel_t pwm_channels[] =
{
    LEDC_CHANNEL_0,
    LEDC_CHANNEL_1,
    LEDC_CHANNEL_2,
    LEDC_CHANNEL_3,
    LEDC_CHANNEL_4,
    LEDC_CHANNEL_6,
};

static const int g_tube_pin_map[] =
{
  // 0   1   2   3   4   5   6   7   8   9  ,    ,
     4, 19, 18, 17, 16, 15,  0,  1,  2,  3, 71, 70,
    10,  5,  6,  7,  8,  9, 14, 13, 12, 11, 69, 68,
    24, 39, 38, 37, 36, 35, 20, 21, 22, 23, 67, 66,
    30, 25, 26, 27, 28, 29, 34, 33, 32, 31, 65, 64,
    44, 59, 58, 57, 56, 55, 40, 41, 42, 43, 63, 62,
    50, 45, 46, 47, 48, 49, 54, 53, 52, 51, 61, 60,
};

CustomNixieDisplay::CustomNixieDisplay()
    : NixieDisplay()
    , m_tubes{}
    , m_cathodes{ SPI, GPIO_NUM_17, 4 }
    , m_anods{ g_anods, pwm_channels, 6, TUBE_PWM_FREQ_HZ }
{
    m_cathodes.set_map( g_tube_pin_map, sizeof(g_tube_pin_map) );
    m_tubes[0].set_cathodes( 0, &m_cathodes );
    m_tubes[0].set_anod( 0, &m_anods );
    m_tubes[1].set_cathodes( 12, &m_cathodes );
    m_tubes[1].set_anod( 1, &m_anods );
    m_tubes[2].set_cathodes( 24, &m_cathodes );
    m_tubes[2].set_anod( 2, &m_anods );
    m_tubes[3].set_cathodes( 36, &m_cathodes );
    m_tubes[3].set_anod( 3, &m_anods );
    m_tubes[4].set_cathodes( 48, &m_cathodes );
    m_tubes[4].set_anod( 4, &m_anods );
    m_tubes[5].set_cathodes( 60, &m_cathodes );
    m_tubes[5].set_anod( 5, &m_anods );
    // Set pwm range to half of available pwm
    m_anods.set_pwm_range( 0, 250 );
}

NixieTubeAnimated* CustomNixieDisplay::get_by_index(int index)
{
    if (index < sizeof(m_tubes) / sizeof(m_tubes[0]))
        return &m_tubes[index];
    return nullptr;
}

void CustomNixieDisplay::begin()
{
    m_anods.begin();
    m_cathodes.begin();
    NixieDisplay::begin();
}

void CustomNixieDisplay::update()
{
    NixieDisplay::update();
    m_anods.update();
    m_cathodes.update();
}

void CustomNixieDisplay::end()
{
    NixieDisplay::end();
    m_cathodes.end();
    m_anods.end();
}

#endif
