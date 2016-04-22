#include "AudioBuffer.h"

namespace df3d {

static char STREAMING_BUFFER[AudioBuffer::STREAMING_CHUNK_SIZE];     // FIXME:

AudioBuffer::AudioBuffer(bool streamed)
{
    size_t buffersCount = streamed ? BUFFERS_COUNT : 1;

    for (size_t i = 0; i < buffersCount; i++)
        alGenBuffers(1, &m_alBuffers[i]);

    printOpenALError();
}

AudioBuffer::~AudioBuffer()
{
    for (size_t i = 0; i < BUFFERS_COUNT; i++)
    {
        if (m_alBuffers[i])
            alDeleteBuffers(1, &m_alBuffers[i]);
    }
}

void AudioBuffer::init(unique_ptr<IAudioStream> stream)
{
    m_stream = std::move(stream);
}

void AudioBuffer::init(unique_ptr<PCMData> pcmData)
{
    alBufferData(m_alBuffers[0], pcmData->format, pcmData->data, pcmData->totalSize, pcmData->sampleRate);
}

void AudioBuffer::attachToSource(ALuint alSourceId)
{
    if (m_stream)
    {
        for (size_t i = 0; i < BUFFERS_COUNT; i++)
        {
            if (m_alBuffers[i])
            {
                if (m_stream->streamData(m_alBuffers[i], STREAMING_CHUNK_SIZE, STREAMING_BUFFER, false))
                    alSourceQueueBuffers(alSourceId, 1, &m_alBuffers[i]);
            }
        }
    }
    else
        alSourcei(alSourceId, AL_BUFFER, m_alBuffers[0]);
}

void AudioBuffer::streamData(ALuint alSourceId, bool looped)
{
    ALint processedBuffers;
    alGetSourcei(alSourceId, AL_BUFFERS_PROCESSED, &processedBuffers);

    while (processedBuffers-- > 0)
    {
        ALuint bufferId;
        alSourceUnqueueBuffers(alSourceId, 1, &bufferId);
        if (m_stream->streamData(bufferId, STREAMING_CHUNK_SIZE, STREAMING_BUFFER, looped))
        {
            alSourceQueueBuffers(alSourceId, 1, &bufferId);
        }
        else
        {
            // EOF
        }
    }

    printOpenALError();
}

}
