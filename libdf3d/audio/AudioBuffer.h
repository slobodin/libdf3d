#pragma once

#include <libdf3d/resources/Resource.h>

namespace df3d {

class AudioBufferFSLoader;

class AudioBuffer : public Resource
{
    friend class AudioBufferFSLoader;

    unsigned m_alBufferId = 0;

    AudioBuffer();

public:
    ~AudioBuffer();

    unsigned getALId() const { return m_alBufferId; }
};

}
