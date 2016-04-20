#include "AudioLoader_wav.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/io/FileDataSource.h>

namespace df3d { namespace resource_loaders_impl {

#pragma pack(push, 1)

struct RIFFHeader
{
    char chunkID[4];
    long chunkSize;
    char format[4];
};

struct WAVEFormat
{
    char subChunkID[4];
    long subChunkSize;
    short audioFormat;
    short numChannels;
    long sampleRate;
    long byteRate;
    short blockAlign;
    short bitsPerSample;
};

struct WAVEData
{
    char subChunkID[4];
    long subChunk2Size;
};

#pragma pack(pop)

unique_ptr<AudioBufferFSLoader::PCMData> AudioLoader_wav::load(shared_ptr<FileDataSource> source)
{
    RIFFHeader header;
    source->getAsObjects(&header, 1);

    if (memcmp(header.chunkID, "RIFF", 4) || memcmp(header.format, "WAVE", 4))
    {
        glog << "Invalid WAVE file header" << source->getPath() << logwarn;
        return nullptr;
    }

    WAVEFormat format;
    source->getAsObjects(&format, 1);

    if (memcmp(format.subChunkID, "fmt ", 4))
    {
        glog << "Invalid WAVE format" << source->getPath() << logwarn;
        return nullptr;
    }

    if (format.subChunkSize > 16)
        source->seek(sizeof(short), std::ios_base::cur);

    while (true)
    {
        WAVEData data;
        if (!source->getAsObjects(&data, 1))
            return nullptr;

        if (memcmp(data.subChunkID, "data", 4) == 0)
        {
            char *sounddata = new char[data.subChunk2Size];

            auto got = source->getRaw(sounddata, data.subChunk2Size);
            if (got != data.subChunk2Size)
            {
                glog << "Error loading WAVE data" << source->getPath() << logwarn;
                return nullptr;
            }

            AudioBufferFSLoader::PCMData::Format fmt;

            if (format.numChannels == 1)
            {
                if (format.bitsPerSample == 8)
                    fmt = AudioBufferFSLoader::PCMData::Format::MONO_8;
                else if (format.bitsPerSample == 16)
                    fmt = AudioBufferFSLoader::PCMData::Format::MONO_16;
            }
            else if (format.numChannels == 2)
            {
                if (format.bitsPerSample == 8)
                    fmt = AudioBufferFSLoader::PCMData::Format::STEREO_8;
                else if (format.bitsPerSample == 16)
                    fmt = AudioBufferFSLoader::PCMData::Format::STEREO_16;
            }

            auto result = make_unique<AudioBufferFSLoader::PCMData>();
            result->format = fmt;
            result->data = sounddata;
            result->dataSize = data.subChunk2Size;
            result->sampleRate = format.sampleRate;

            return result;
        }
        else
        {
            source->seek(data.subChunk2Size, std::ios_base::cur);
        }
    }

    return nullptr;
}

} }

