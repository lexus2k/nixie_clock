#include "pin_muxers.h"
#include "clock_display.h"
#include "spibus.h"
#include "hv5812.h"
#include "clock_hardware.h"
#include "hv5812_group_controller.h"

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

static int g_tube_pin_map[] =
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
    , m_tubes{ {0}, {12}, {24}, {36}, {48}, {60} }
    , m_cathodes{ SPI, GPIO_NUM_17, 4 }
{
}

NixieTubeAnimated* CustomNixieDisplay::get_by_index(int index)
{
    if (index < sizeof(m_tubes) / sizeof(m_tubes[0]))
        return &m_tubes[index];
    return nullptr;
}

void CustomNixieDisplay::begin()
{
    m_cathodes.set_map( g_tube_pin_map, sizeof(g_tube_pin_map) );
    display.set_cathodes( &m_cathodes );
    display.set_anods(g_anods);

    // Init ledc timer: TODO: to make as part of display initialization
    display.enable_pwm( pwm_channels );
    NixieDisplay::begin();
}

void CustomNixieDisplay::update()
{
    NixieDisplay::update();
    m_cathodes.update();
}

void CustomNixieDisplay::end()
{
    NixieDisplay::end();
}

#endif
