#pragma once

#include "nixie_tube_animated.h"

class NixieTubeIn14: public NixieTubeAnimated
{
public:
    using NixieTubeAnimated::NixieTubeAnimated;
    /**
     * Accepts '~', '.', ',', ' '
     */
    const char * set(const char *p, bool apply = true) override;
};

