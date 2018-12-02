#pragma once

#include "sm_state.h"

class StateInit: public SmState
{
public:
    StateInit(): SmState("init") {}

    void enter() override;
    void run() override;

protected:
    uint8_t get_id() override;

private:
    uint32_t m_start_us;
};

