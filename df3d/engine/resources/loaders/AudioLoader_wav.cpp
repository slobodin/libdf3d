#include "AudioLoader_wav.h"

#include <df3d/engine/audio/OpenALCommon.h>
#include <df3d/engine/resources/AudioResource.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceFileSystem.h>
#include <df3d/engine/resources/ResourceDataSource.h>

namespace df3d {

#pragma pack(push, 1)

struct RIFFHeader
{
    char chunkID[4];
    uint32_t chunkSize;
    char format[4];
};

struct WAVEFormat
{
    char subChunkID[4];
    uint32_t subChunkSize;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
};

struct WAVEData
{
    char subChunkID[4];
    uint32_t subChunk2Size;
};

#pragma pack(pop)

unique_ptr<PCMData> AudioLoader_wav(const char *path, Allocator &alloc)
{
    auto dataSource = svc().resourceManager().getFS().open(path);
    if (!dataSource)
        return nullptr;

    RIFFHeader header;
    dataSource->getObjects(&header, 1);

    if (strncmp(header.chunkID, "RIFF", 4) != 0 || strncmp(header.format, "WAVE", 4) != 0)
    {
        DFLOG_WARN("Invalid WAVE file header: %s", path);
        svc().resourceManager().getFS().close(dataSource);
        return nullptr;
    }

    WAVEFormat format;
    dataSource->getObjects(&format, 1);

    if (memcmp(format.subChunkID, "fmt ", 4))
    {
        DFLOG_WARN("Invalid WAVE format: %s", path);
        svc().resourceManager().getFS().close(dataSource);
        return nullptr;
    }

    if (format.subChunkSize > 16)
        dataSource->seek(sizeof(short), SeekDir::CURRENT);

    auto result = make_unique<PCMData>(alloc);

    while (true)
    {
        WAVEData data;
        if (!dataSource->getObjects(&data, 1))
        {
            svc().resourceManager().getFS().close(dataSource);
            return nullptr;
        }

        if (memcmp(data.subChunkID, "data", 4) == 0)
        {
            result->data.resize(data.subChunk2Size);
            auto got = dataSource->read(result->data.data(), data.subChunk2Size);
            if (got != data.subChunk2Size)
            {
                DFLOG_WARN("Error loading WAVE data: %s", path);
                svc().resourceManager().getFS().close(dataSource);
                return nullptr;
            }

            if (format.numChannels == 1)
            {
                if (format.bitsPerSample == 8)
                    result->format = AL_FORMAT_MONO8;
                else if (format.bitsPerSample == 16)
                    result->format = AL_FORMAT_MONO16;
            }
            else if (format.numChannels == 2)
            {
                if (format.bitsPerSample == 8)
                    result->format = AL_FORMAT_STEREO8;
                else if (format.bitsPerSample == 16)
                    result->format = AL_FORMAT_STEREO16;
            }

            result->sampleRate = format.sampleRate;

            svc().resourceManager().getFS().close(dataSource);
            return result;
        }
        else
        {
            dataSource->seek(data.subChunk2Size, SeekDir::CURRENT);
        }
    }

    svc().resourceManager().getFS().close(dataSource);
    return nullptr;
}

}
