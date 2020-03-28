#pragma once

#include "sm_engine.h"
#include <time.h>

class StateMain: public SmState
{
public:
    StateMain(): SmState("main") {}

    void enter() override;

    void update() override;

    EEventResult on_event(SEventData event) override;

private:
    struct tm m_last_tm_info{};

    void on_highlight_toggle();
    void on_change_highlight();
};

