#pragma once

#include "sm_state.h"
#include <time.h>

class StateTimeSetup: public SmState
{
public:
    StateTimeSetup(): SmState("set_time") {}

    void enter() override;
    void run() override;
    void exit() override;
    EEventResult on_event(SEventData event) override;
private:
    uint8_t m_state;
    struct tm m_time_info;

    void update_display_content();
    void move_to_next_position();
    void increase_value();
    void decrease_value();
    void save_time();
};

