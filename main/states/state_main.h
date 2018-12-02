#pragma once

#include "sm_engine.h"
#include "sm_state.h"
#include <time.h>

class StateMain: public SmState
{
public:
    StateMain(): SmState("main") {}

    void enter() override;
    void run() override;
    bool on_event(SEventData event) override;
    uint8_t get_id() override;
private:
    struct tm m_last_tm_info{};
};

