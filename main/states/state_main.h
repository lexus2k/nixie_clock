#pragma once

#include "sme/engine.h"
#include <time.h>

class StateMain: public SmState
{
public:
    StateMain(): SmState("main") {}

    void enter(SEventData *event) override;

    void update() override;

    STransitionData onEvent(SEventData event) override;

private:
    struct tm m_last_tm_info{};
    bool m_cooldown_alarm;

    void on_highlight_toggle();
    void on_change_highlight();
};

