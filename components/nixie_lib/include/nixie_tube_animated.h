#pragma once

#include "nixie_tube_base.h"

enum class Effect: uint8_t
{
    IMMEDIATE,
    SCROLL,
    OVERLAP,
};

class NixieTubeAnimated: public NixieTubeBase
{
public:
    using NixieTubeBase::NixieTubeBase;

    void begin();
    void update() override;
    void end();

    /**
     * Accepts string, parses it and returns pointer to next
     * not accepted character.
     * if char contains '~' that means no state change
     * if char contains ',' or '.' that means dot
     * if char contains ' ' that means turn off the tube digit
     */
    virtual const char * set(const char *p) = 0;
#if 0
    void copy_from(const NixieTubeAnimated& tube);
#endif
    void set_effect(Effect effect);

protected:
    struct
    {
        Effect   effect = Effect::IMMEDIATE;
        uint8_t  index = 0;
        uint64_t timestamp_us = 0;
        int      extra;
        int      target_value = 0;
        int      value = 0;
    } m_state;

    uint64_t m_last_us = 0;
    void animate(int value);

private:
    void set(int digit);
    void scroll(int value);
    void overlap(int value);
    void do_scroll();
    void do_overlap();
};

