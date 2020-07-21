#pragma once

#include "nixie_tube_in12a.h"

class NixieTubeIn12A_Dots:public NixieTubeIn12A
{
public:
    using NixieTubeIn12A::NixieTubeIn12A;

    /* requires 3 positions: left dot, digit, and right dot */
    void set(const char *p) override;

    std::string get_content() override;
};


class NixieTubeIn12A_NoDots:public NixieTubeIn12A
{
public:
    using NixieTubeIn12A::NixieTubeIn12A;

    /* requires 3 positions: left dot, digit, and right dot */
    void set(const char *p) override;

    std::string get_content() override;
};
