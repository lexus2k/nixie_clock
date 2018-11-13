#pragma once

#include "nixie_tube_base.h"

class NixieTubeAnimated: public NixieTubeBase
{
public:
    using NixieTubeBase::NixieTubeBase;

    void begin();
    void update() override;
    void end();

    void set(int digit);
//    void set(char ch);
#if 0
    // TODO
    virtual char* set(const char* str) = 0;
    void copy_from(const NixieTubeAnimated& tube);
#endif
    void scroll(int value);
    void overlap(int value);

protected:
    struct
    {
        uint8_t  index = 0;
        uint64_t timestamp_us = 0;
        int      extra;
        int      target_value = 0;
        int      value = 0;
    } m_state;

    uint64_t m_last_us = 0;

private:
    void do_scroll();
    void do_overlap();
};

