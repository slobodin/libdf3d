#include "df3d_pch.h"
#include "AudioBuffer.h"

#include "OpenALCommon.h"

namespace df3d { namespace audio {

void AudioBuffer::onDecoded(bool decodeResult)
{
    m_initialized = decodeResult;
}

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

} }
