#pragma once

#include "nixie_tube_animated.h"

class NixieTubeIn14: public NixieTubeAnimated
{
public:
    using NixieTubeAnimated::NixieTubeAnimated;

    void set(const char* str) override;
};

