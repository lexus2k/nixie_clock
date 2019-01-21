#include "pin_muxers.h"
#include "clock_display.h"
#include "spibus.h"
#include "hv5812.h"
#include "clock_hardware.h"
#include "hv5812_group_controller.h"
#include "nixie_tube_in14.h"
#include "tubes/in12tubes.h"

#define TUBE_PWM_FREQ_HZ (200)

CustomNixieDisplay::CustomNixieDisplay()
    : NixieDisplay()
    , m_cathodes{ SPI }
    , m_anods{ }
{
}

CustomNixieDisplay::~CustomNixieDisplay()
{
    for (auto tube: m_tubes)
    {
        delete tube;
    }
    m_tubes.clear();
}

void CustomNixieDisplay::setup_in14()
{
    m_cathodes.setup( GPIO_NUM_17,
                      {
                     // 0   1   2   3   4   5   6   7   8   9  ,    ,
                        4, 19, 18, 17, 16, 15,  0,  1,  2,  3, 71, 70,
                       10,  5,  6,  7,  8,  9, 14, 13, 12, 11, 69, 68,
                       24, 39, 38, 37, 36, 35, 20, 21, 22, 23, 67, 66,
                       30, 25, 26, 27, 28, 29, 34, 33, 32, 31, 65, 64,
                       44, 59, 58, 57, 56, 55, 40, 41, 42, 43, 63, 62,
                       50, 45, 46, 47, 48, 49, 54, 53, 52, 51, 61, 60,
                      }
                    );
    m_anods.setup( { {GPIO_NUM_12, LEDC_CHANNEL_0},
                     {GPIO_NUM_14, LEDC_CHANNEL_1},
                     {GPIO_NUM_27, LEDC_CHANNEL_2},
                     {GPIO_NUM_33, LEDC_CHANNEL_3},
                     {GPIO_NUM_32, LEDC_CHANNEL_4},
                     {GPIO_NUM_16, LEDC_CHANNEL_6},
                   }, TUBE_PWM_FREQ_HZ );
    // Set pwm range.
    // Max output power is 6*174V*0.0025A*upper_range/1023
    // 330 pwm means around 0.84Wt power consumption per 6 tubes
    // 440 pwm means around 1.12Wt power consumption per 6 tubes
    // increasing value more can damange MOSFET
    m_anods.set_pwm_range( 0, 500 );
    for (int i=0; i<6; i++)
    {
        m_tubes.emplace_back(new NixieTubeIn14());
    }
    m_tubes[0]->set_cathodes( 0, &m_cathodes );
    m_tubes[0]->set_anod( 0, &m_anods );
    m_tubes[1]->set_cathodes( 12, &m_cathodes );
    m_tubes[1]->set_anod( 1, &m_anods );
    m_tubes[2]->set_cathodes( 24, &m_cathodes );
    m_tubes[2]->set_anod( 2, &m_anods );
    m_tubes[3]->set_cathodes( 36, &m_cathodes );
    m_tubes[3]->set_anod( 3, &m_anods );
    m_tubes[4]->set_cathodes( 48, &m_cathodes );
    m_tubes[4]->set_anod( 4, &m_anods );
    m_tubes[5]->set_cathodes( 60, &m_cathodes );
    m_tubes[5]->set_anod( 5, &m_anods );
}

void CustomNixieDisplay::setup_in12a()
{
    m_cathodes.setup( GPIO_NUM_17,
                      {
                     // 0   1   2   3   4   5   6   7   8   9   ,
                        0, 15, 16, 17, 18, 19,  4,  3,  2,  1,
                       14,  9,  8,  7,  6,  5, 10, 11, 12, 13, 60,
                       20, 35, 36, 37, 38, 39, 24, 23, 22, 21,
                       34, 29, 28, 27, 26, 25, 30, 31, 32, 33, 61,
                       40, 55, 56, 57, 58, 59, 44, 43, 42, 41,
                       54, 49, 48, 47, 46, 45, 50, 51, 52, 53,
                      }
                    );
    m_anods.setup( { {GPIO_NUM_12, LEDC_CHANNEL_0},
                     {GPIO_NUM_14, LEDC_CHANNEL_1},
                     {GPIO_NUM_27, LEDC_CHANNEL_2},
                     {GPIO_NUM_33, LEDC_CHANNEL_3},
                     {GPIO_NUM_32, LEDC_CHANNEL_4},
                     {GPIO_NUM_16, LEDC_CHANNEL_6},
                   }, TUBE_PWM_FREQ_HZ );
    // Set pwm range.
    // Max output power is 6*174V*0.0025A*upper_range/1023
    // 330 pwm means around 0.84Wt power consumption per 6 tubes
    // 440 pwm means around 0.84Wt power consumption per 6 tubes
    // increasing value more can damange MOSFET
    m_anods.set_pwm_range( 0, 400 );
    m_tubes.emplace_back(new NixieTubeIn12A_NoDots());
    m_tubes.emplace_back(new NixieTubeIn12A_Dots());
    m_tubes.emplace_back(new NixieTubeIn12A_NoDots());
    m_tubes.emplace_back(new NixieTubeIn12A_Dots());
    m_tubes.emplace_back(new NixieTubeIn12A_NoDots());
    m_tubes.emplace_back(new NixieTubeIn12A_NoDots());
    m_tubes[0]->set_cathodes( 0, &m_cathodes );
    m_tubes[0]->set_anod( 0, &m_anods );
    m_tubes[1]->set_cathodes( 10, &m_cathodes );
    m_tubes[1]->set_anod( 1, &m_anods );
    m_tubes[2]->set_cathodes( 21, &m_cathodes );
    m_tubes[2]->set_anod( 2, &m_anods );
    m_tubes[3]->set_cathodes( 31, &m_cathodes );
    m_tubes[3]->set_anod( 3, &m_anods );
    m_tubes[4]->set_cathodes( 42, &m_cathodes );
    m_tubes[4]->set_anod( 4, &m_anods );
    m_tubes[5]->set_cathodes( 52, &m_cathodes );
    m_tubes[5]->set_anod( 5, &m_anods );
}


NixieTubeAnimated* CustomNixieDisplay::get_by_index(int index)
{
    if (index < m_tubes.size())
        return m_tubes[index];
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
    // TODO: rework this place
    // Actually anods are already updated elsewhere, if they use PWM.
    // Cathodes are updated much later, this can cause tube gas lighting.
    NixieDisplay::update();
    m_cathodes.update();
    m_anods.update();
}

void CustomNixieDisplay::end()
{
    NixieDisplay::end();
    m_cathodes.end();
    m_anods.end();
}

