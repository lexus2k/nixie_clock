#pragma once
#include "nixie_tube_animated.h"

class NixieTubeFake: public NixieTubeAnimated
{
public:
    void set(const char *p) override {};
};
