#include "AudioBuffer.h"

#include "OpenALCommon.h"

namespace df3d {

AudioBuffer::AudioBuffer()
{
    alGenBuffers(1, &m_alBufferId);

    printOpenALError();
}

AudioBuffer::~AudioBuffer()
{
    if (m_alBufferId)
        alDeleteBuffers(1, &m_alBufferId);
}

}
