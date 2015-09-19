#include "AudioLoaders.h"

#include <audio/OpenALCommon.h>
#include <base/Service.h>
#include <resources/FileSystem.h>
#include <resources/FileDataSource.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace df3d { namespace resources {

namespace wav {

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

unique_ptr<AudioBufferFSLoader::PCMData> loadPCM(shared_ptr<FileDataSource> source)
{
    RIFFHeader header;
    source->getAsObjects(&header, 1);

    if (memcmp(header.chunkID, "RIFF", 4) || memcmp(header.format, "WAVE", 4))
    {
        base::glog << "Invalid WAVE file header" << source->getPath() << base::logwarn;
        return nullptr;
    }

    WAVEFormat format;
    source->getAsObjects(&format, 1);

    if (memcmp(format.subChunkID, "fmt ", 4))
    {
        base::glog << "Invalid WAVE format" << source->getPath() << base::logwarn;
        return nullptr;
    }

    if (format.subChunkSize > 16)
        source->seek(sizeof(short), std::ios_base::cur);

    WAVEData data;
    source->getAsObjects(&data, 1);

    if (memcmp(data.subChunkID, "data", 4))
    {
        base::glog << "Invalid WAVE data" << source->getPath() << base::logwarn;
        return nullptr;
    }

    char *sounddata = new char[data.subChunk2Size];

    auto got = source->getRaw(sounddata, data.subChunk2Size);
    if (got != data.subChunk2Size)
    {
        base::glog << "Error loading WAVE data" << source->getPath() << base::logwarn;
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

}

namespace ogg {

size_t readOgg(void *ptr, size_t size, size_t nmemb, void *datasource)
{
    FileDataSource *file = reinterpret_cast<FileDataSource *>(datasource);

    return file->getRaw(ptr, size * nmemb);
}

int seekOgg(void *datasource, ogg_int64_t offset, int whence)
{
    FileDataSource *file = reinterpret_cast<FileDataSource *>(datasource);

    bool res;
    switch (whence)
    {
    case SEEK_SET:
        res = file->seek((long)offset, std::ios_base::beg);
        break;
    case SEEK_CUR:
        res = file->seek((long)offset, std::ios_base::cur);
        break;
    case SEEK_END:
        res = file->seek((long)offset, std::ios_base::end);
        break;
    default:
        return -1;
    }

    return res ? 0 : -1;
}

long tellOgg(void *datasource)
{
    FileDataSource *file = reinterpret_cast<FileDataSource *>(datasource);
    return file->tell();
}

int closeOgg(void *datasource)
{
    return 0;
}

unique_ptr<AudioBufferFSLoader::PCMData> loadPCM(shared_ptr<FileDataSource> source)
{
    // TODO:
    // Streaming.

    ov_callbacks ovCallbacks;
    ovCallbacks.close_func = closeOgg;
    ovCallbacks.read_func = readOgg;
    ovCallbacks.seek_func = seekOgg;
    ovCallbacks.tell_func = tellOgg;

    OggVorbis_File oggVorbisFile;

    if (ov_open_callbacks(source.get(), &oggVorbisFile, NULL, -1, ovCallbacks) < 0)
    {
        base::glog << "Failed to open ogg file" << source->getPath() << base::logwarn;
        return nullptr;
    }

    auto ovInfo = ov_info(&oggVorbisFile, -1);
    long pcmSize = (long)ov_pcm_total(&oggVorbisFile, -1) * ovInfo->channels * 2;

    // Read PCM.
    auto audioData = new char[pcmSize];
    long totalGot = 0, got;
    int currentSection;

    while (totalGot < pcmSize)
    {
        got = (long)ov_read(&oggVorbisFile, audioData + totalGot, pcmSize - totalGot, 0, 2, 1, &currentSection);

        if (got == 0)
            break;
        else
            totalGot += got;
    }

    auto result = make_unique<AudioBufferFSLoader::PCMData>();
    result->format = ovInfo->channels == 1 ? AudioBufferFSLoader::PCMData::Format::MONO_16 : AudioBufferFSLoader::PCMData::Format::STEREO_16;
    result->data = audioData;
    result->dataSize = totalGot;
    result->sampleRate = ovInfo->rate;

    ov_clear(&oggVorbisFile);

    return result;
}

}

ALenum convertALFormat(AudioBufferFSLoader::PCMData::Format format)
{
    switch (format)
    {
    case AudioBufferFSLoader::PCMData::Format::MONO_8:
        return AL_FORMAT_MONO8;
    case AudioBufferFSLoader::PCMData::Format::MONO_16:
        return AL_FORMAT_MONO16;
    case AudioBufferFSLoader::PCMData::Format::STEREO_8:
        return AL_FORMAT_STEREO8;
    case AudioBufferFSLoader::PCMData::Format::STEREO_16:
        return AL_FORMAT_STEREO16;
    default:
        break;
    }

    return AL_INVALID_ENUM;
}

AudioBufferFSLoader::AudioBufferFSLoader(const std::string &path)
    : FSResourceLoader(ResourceLoadingMode::IMMEDIATE),      // FIXME: can do streaming?
    m_path(path)
{

}

audio::AudioBuffer* AudioBufferFSLoader::createDummy()
{
    return new audio::AudioBuffer();
}

void AudioBufferFSLoader::decode(shared_ptr<FileDataSource> source)
{
    auto extension = gsvc().filesystem.getFileExtension(source->getPath());

    if (extension == ".wav")
        m_pcmData = wav::loadPCM(source);
    else if (extension == ".ogg")
        m_pcmData = ogg::loadPCM(source);
}

void AudioBufferFSLoader::onDecoded(Resource *resource)
{
    auto audioBuffer = static_cast<audio::AudioBuffer*>(resource);

    alBufferData(audioBuffer->getALId(), convertALFormat(m_pcmData->format), m_pcmData->data, m_pcmData->dataSize, m_pcmData->sampleRate);

    // Explicitly remove PCM buffer in order to prevent caching.
    m_pcmData.reset();

    audioBuffer->m_initialized = true;

    printOpenALError();
}

} }
