#pragma once

#include "nixie_tube_animated.h"

class NixieTubeIn12A: public NixieTubeAnimated
{
public:
    using NixieTubeAnimated::NixieTubeAnimated;
    /**
     * Accepts '~', ' '
     */
    void set(const char *p) override;
};

