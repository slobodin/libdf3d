#pragma once

#include <resources/Resource.h>

namespace df3d { namespace audio {

class AudioBuffer : public resources::Resource
{
    unsigned m_alBufferId = 0;

public:
    AudioBuffer(unsigned bufferId);
    ~AudioBuffer();

    bool init();
    unsigned getALId() const { return m_alBufferId; }
};

} }