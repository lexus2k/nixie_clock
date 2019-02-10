#pragma once

#include "nixie_tube_base.h"
#include <string>

class NixieTubeAnimated: public NixieTubeBase
{
public:
    enum class Effect: uint8_t
    {
        IMMEDIATE,
        SCROLL,
        OVERLAP,
        LAST,
    };

    using NixieTubeBase::NixieTubeBase;

    void begin();
    void update() override;
    void end();

    using NixieTubeBase::off;
    void off(uint32_t delay_us);

    /**
     * Accepts string, parses it and returns pointer to next
     * not accepted character.
     * if char contains '~' that means no state change
     * if char contains ',' or '.' that means dot
     * if char contains ' ' that means turn off the tube digit
     */
    virtual void set(const char *p) = 0;
    std::string get_content();
    void set_effect(NixieTubeAnimated::Effect effect);

protected:
    struct
    {
        NixieTubeAnimated::Effect   effect = NixieTubeAnimated::Effect::IMMEDIATE;
        uint8_t  index = 0;
        uint64_t timestamp_us = 0;
        int      extra;
        int      target_value = 0;
        int      value = 0;
    } m_state;

    uint64_t m_last_us = 0;
    uint64_t m_off_us = 0;
    void animate(int value);

private:
    void set(int digit);
    void scroll(int value);
    void overlap(int value);
    void do_scroll();
    void do_overlap();
};

