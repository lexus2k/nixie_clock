#pragma once

#include "sm_engine.h"
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
    uint8_t get_id() override;
private:
    uint8_t m_state;
    uint32_t m_start_us;
    struct tm m_time_info;

    void update_display_content();
};

