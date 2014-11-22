#include "df3d_pch.h"
#include "DecoderWAV.h"

#include <resources/FileDataSource.h>
#include <audio/AudioBuffer.h>
#include <audio/OpenALCommon.h>

namespace df3d { namespace resources {

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

DecoderWAV::DecoderWAV()
{
}

DecoderWAV::~DecoderWAV()
{
}

shared_ptr<Resource> DecoderWAV::createResource()
{
    ALuint bufferId = 0;
    alGenBuffers(1, &bufferId);

    printOpenALError();

    return make_shared<audio::AudioBuffer>(bufferId);
}

bool DecoderWAV::decodeResource(const shared_ptr<FileDataSource> file, shared_ptr<Resource> resource)
{
    if (!file || !file->valid())
        return false;

    auto buffer = dynamic_pointer_cast<audio::AudioBuffer>(resource);
    if (!buffer)
        return false;

    RIFFHeader header;
    file->getAsObjects(&header, 1);

    if (memcmp(header.chunkID, "RIFF", 4) || memcmp(header.format, "WAVE", 4))
    {
        base::glog << "Invalid WAVE file header" << resource->getGUID() << base::logwarn;
        return false;
    }

    WAVEFormat format;
    file->getAsObjects(&format, 1);

    if (memcmp(format.subChunkID, "fmt ", 4))
    {
        base::glog << "Invalid WAVE format" << resource->getGUID() << base::logwarn;
        return false;
    }

    if (format.subChunkSize > 16)
        file->seek(sizeof(short), std::ios_base::cur);

    WAVEData data;
    file->getAsObjects(&data, 1);

    if (memcmp(data.subChunkID, "data", 4))
    {
        base::glog << "Invalid WAVE data" << resource->getGUID() << base::logwarn;
        return false;
    }

    unsigned char *sounddata = new unsigned char[data.subChunk2Size];

    auto got = file->getRaw(sounddata, data.subChunk2Size);
    if (got != data.subChunk2Size)
    {
        base::glog << "Error loading WAVE data" << resource->getGUID() << base::logwarn;
        return false;
    }

    ALenum alFormat = AL_INVALID_ENUM;

    if (format.numChannels == 1)
    {
        if (format.bitsPerSample == 8)
            alFormat = AL_FORMAT_MONO8;
        else if (format.bitsPerSample == 16)
            alFormat = AL_FORMAT_MONO16;
    }
    else if (format.numChannels == 2)
    {
        if (format.bitsPerSample == 8)
            alFormat = AL_FORMAT_STEREO8;
        else if (format.bitsPerSample == 16)
            alFormat = AL_FORMAT_STEREO16;
    }

    alBufferData(buffer->getALId(), alFormat, sounddata, data.subChunk2Size, format.sampleRate);

    delete [] sounddata;

    printOpenALError();

    return true;
}

} }