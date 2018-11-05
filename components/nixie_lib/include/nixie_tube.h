#pragma once

#include "nixie_tube_in14.h"
#include "nixie_tube_in3.h"

class NixieTube: public NixieTubeIn14
{
public:
    using NixieTubeIn14::NixieTubeIn14;
};

