#pragma once

#include "sme/engine.h"
#include <time.h>

class StateAlarmSetup: public SmState
{
public:
    StateAlarmSetup(): SmState("set_alarm") {}

    void enter(SEventData *event) override;

    void update() override;

    void exit(SEventData *event) override;

    STransitionData onEvent(SEventData event) override;

private:
    uint8_t m_state;
    struct tm m_time_info;
    bool m_enabled;

    void update_display_content();
    void move_to_next_position();
    void increase_value();
    void decrease_value();
    void save_time();
};

