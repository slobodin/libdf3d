#pragma once

#include <libdf3d/resources/Resource.h>
#include "impl/OpenALCommon.h"

namespace df3d {

class AudioBufferFSLoader;

class IAudioStream
{
public:
    IAudioStream() = default;
    virtual ~IAudioStream() = default;

    virtual bool streamData(ALuint alBuffer, long dataSize, char *buffer, bool looped) = 0;
};

struct PCMData
{
    char *data = nullptr;
    size_t totalSize = 0;
    size_t sampleRate = 0;
    ALuint format;

    ~PCMData() 
    { 
        delete[] data; 
    }
};

class AudioBuffer : public Resource
{
public:
    static const long STREAMING_CHUNK_SIZE = 48000;
    static const size_t BUFFERS_COUNT = 3;

private:
    friend class AudioBufferFSLoader;

    ALenum m_format = AL_INVALID_ENUM;
    ALuint m_alBuffers[BUFFERS_COUNT] = { 0 };      // FIXME: hardcoded.

    unique_ptr<IAudioStream> m_stream;

    AudioBuffer(bool streamed);

public:
    ~AudioBuffer();

    void init(unique_ptr<IAudioStream> stream);
    void init(unique_ptr<PCMData> pcmData);

    void attachToSource(ALuint alSourceId);

    void streamData(ALuint alSourceId, bool looped);

    bool isStreamed() const { return m_stream != nullptr; }
};

}
