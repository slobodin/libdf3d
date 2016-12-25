#include "AudioResource.h"

#include <df3d/engine/audio/OpenALCommon.h>
#include <df3d/lib/JsonUtils.h>
#include <df3d/engine/io/FileSystemHelpers.h>
#include "loaders/AudioLoader_wav.h"
#include "loaders/AudioLoader_ogg.h"

namespace df3d {

static const size_t STREAMING_CHUNK_SIZE = 48000;
static const size_t BUFFERS_COUNT = 3;
static char STREAMING_BUFFER[STREAMING_CHUNK_SIZE]; // FIXME:

class AudioBuffer
{
    ALuint m_alBuffers[BUFFERS_COUNT] = { 0 };      // FIXME: hardcoded.
    unique_ptr<IAudioStream> m_stream;

    void initCommon(bool streamed)
    {
        size_t buffersCount = streamed ? BUFFERS_COUNT : 1;

        for (size_t i = 0; i < buffersCount; i++)
            alGenBuffers(1, &m_alBuffers[i]);

        printOpenALError();
    }

public:
    AudioBuffer(const PCMData &pcmData)
    {
        initCommon(false);

        alBufferData(m_alBuffers[0], pcmData.format, pcmData.data.data(), pcmData.data.size(), pcmData.sampleRate);
    }

    AudioBuffer(unique_ptr<IAudioStream> &&stream)
        : m_stream(std::move(stream))
    {
        initCommon(true);
    }

    ~AudioBuffer()
    {
        for (size_t i = 0; i < BUFFERS_COUNT; i++)
        {
            if (m_alBuffers[i])
                alDeleteBuffers(1, &m_alBuffers[i]);
        }

        printOpenALError();
    }

    void attachToSource(ALuint alSourceId) const
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

    void streamData(ALuint alSourceId, bool looped) const
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

    bool isStreamed() const { return m_stream != nullptr; }
};

void AudioResource::attachToSource(unsigned int alSourceId) const
{
    buffer->attachToSource(alSourceId);
}

void AudioResource::streamData(unsigned int alSourceId, bool looped) const
{
    buffer->streamData(alSourceId, looped);
}

bool AudioResource::isStreamed() const
{
    return buffer->isStreamed();
}

bool AudioResourceHolder::decodeStartup(ResourceDataSource &dataSource, Allocator &allocator)
{
    Json::Value root = JsonUtils::fromFile(dataSource);
    if (root.isNull())
        return false;

    DF3D_ASSERT(root.isMember("path"));

    bool streamed = false;
    root["stream"] >> streamed;
    root["gain"] >> m_gain;
    root["rolloff"] >> m_rolloff;

    const char *path = root["path"].asCString();

    if (FileSystemHelpers::compareExtension(path, ".wav"))
    {
        DF3D_ASSERT_MESS(!streamed, "streaming for wav is unsupported for now");   // TODO:
        m_pcmData = AudioLoader_wav(path, allocator);
        return m_pcmData != nullptr;
    }
    else if (FileSystemHelpers::compareExtension(path, ".ogg"))
    {
        if (streamed)
        {
            m_stream = AudioLoader_ogg_streamed(path, allocator);
            return m_stream != nullptr;
        }
        else
        {
            m_pcmData = AudioLoader_ogg(path, allocator);
            return m_pcmData != nullptr;
        }
    }

    return false;
}

void AudioResourceHolder::decodeCleanup(Allocator &allocator)
{
    m_pcmData.reset();
}

bool AudioResourceHolder::createResource(Allocator &allocator)
{
    m_resource = MAKE_NEW(allocator, AudioResource);
    m_resource->gain = m_gain;
    m_resource->rolloff = m_rolloff;
    if (m_stream)
    {
        m_resource->buffer = MAKE_NEW(allocator, AudioBuffer)(std::move(m_stream));
    }
    else
    {
        m_resource->buffer = MAKE_NEW(allocator, AudioBuffer)(*m_pcmData);
    }

    return true;
}

void AudioResourceHolder::destroyResource(Allocator &allocator)
{
    MAKE_DELETE(allocator, m_resource->buffer);
    MAKE_DELETE(allocator, m_resource);
    m_resource = nullptr;
}

}
