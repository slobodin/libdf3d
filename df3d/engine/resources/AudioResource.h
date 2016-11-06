#pragma once

#include "IResourceHolder.h"

namespace df3d {

class AudioBuffer;

class IAudioStream
{
public:
    IAudioStream() = default;
    virtual ~IAudioStream() = default;

    virtual bool streamData(unsigned int alSourceId, int32_t dataSize, char *buffer, bool looped) = 0;
};

struct PCMData
{
    PodArray<char> data;
    uint32_t sampleRate = 0;
    int format;

    PCMData(Allocator &allocator) : data(allocator) { }
};

struct AudioResource
{
    AudioBuffer *buffer = nullptr;
    float gain = 1.0f;
    float rolloff = 1.0f;

    void attachToSource(unsigned int alSourceId) const;
    void streamData(unsigned int alSourceId, bool looped) const;
    bool isStreamed() const;
};

class AudioResourceHolder : public IResourceHolder
{
    float m_gain = 1.0;
    float m_rolloff = 1.0f;
    AudioResource *m_resource = nullptr;
    unique_ptr<PCMData> m_pcmData;
    unique_ptr<IAudioStream> m_stream;

public:
    void listDependencies(ResourceDataSource &dataSource, std::vector<ResourceID> &outDeps) override { }
    bool decodeStartup(ResourceDataSource &dataSource, Allocator &allocator) override;
    void decodeCleanup(Allocator &allocator) override;
    bool createResource(Allocator &allocator) override;
    void destroyResource(Allocator &allocator) override;

    void* getResource() override { return m_resource; }
};

}
