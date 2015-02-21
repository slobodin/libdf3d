#include "df3d_pch.h"
#include "AudioBuffer.h"

#include "OpenALCommon.h"

namespace df3d { namespace audio {

AudioBuffer::AudioBuffer(unsigned bufferId)
    : m_alBufferId(bufferId)
{
}

AudioBuffer::~AudioBuffer()
{
    if (m_alBufferId)
        alDeleteBuffers(1, &m_alBufferId);
}

} }
