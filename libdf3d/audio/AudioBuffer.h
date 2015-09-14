#pragma once

#include <resources/Resource.h>

FWD_MODULE_CLASS(resources, AudioBufferFSLoader)

namespace df3d { namespace audio {

class AudioBuffer : public resources::Resource
{
    friend class resources::AudioBufferFSLoader;

    unsigned m_alBufferId = 0;

public:
    AudioBuffer();
    ~AudioBuffer();

    unsigned getALId() const { return m_alBufferId; }
};

} }
