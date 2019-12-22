#pragma once

#include "sm_state.h"
#include <time.h>

class StateMain: public SmState
{
public:
    StateMain(): SmState("main") {}

    void enter() override;
    void run() override;
    EEventResult on_event(SEventData event) override;

private:
    struct tm m_last_tm_info{};
};

