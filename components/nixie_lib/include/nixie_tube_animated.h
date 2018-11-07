#pragma once

#include "nixie_tube_base.h"

class NixieTubeAnimated: public NixieTubeBase
{
public:
    using NixieTubeBase::NixieTubeBase;

    void begin();
    void update() override;
    void end();

    virtual void set(int digit);
    void scroll(int value);
    void overlap(int value);

protected:
    uint8_t  m_state = 0;
    uint64_t m_state_us = 0;
    int      m_state_extra;

    uint64_t m_last_us = 0;

private:
    int m_target_value = 0;
    int m_value = 0;

    void do_scroll();
    void do_overlap();
};

