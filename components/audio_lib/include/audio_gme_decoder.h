#pragma once

#include "audio_decoder.h"
#include <stdint.h>

class AudioGmeDecoder: public AudioDecoder
{
public:
    AudioGmeDecoder() = default;
    virtual ~AudioGmeDecoder() = default;

    int decode(uint8_t* buffer, int max_size) override;

};
