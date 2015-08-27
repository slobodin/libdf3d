#include "df3d_pch.h"
#include "DecoderOGG.h"

#include <resources/FileDataSource.h>
#include <resources/FileSystem.h>
#include <audio/AudioBuffer.h>
#include <audio/OpenALCommon.h>

#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace df3d { namespace resources {

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

DecoderOGG::DecoderOGG()
{
}

DecoderOGG::~DecoderOGG()
{
}

shared_ptr<Resource> DecoderOGG::createResource()
{
    ALuint bufferId = 0;
    alGenBuffers(1, &bufferId);

    printOpenALError();

    return make_shared<audio::AudioBuffer>(bufferId);
}

bool DecoderOGG::decodeResource(shared_ptr<FileDataSource> file, shared_ptr<Resource> resource)
{
    if (!file || !file->valid())
        return false;

    auto audiobuffer = dynamic_pointer_cast<audio::AudioBuffer>(resource);
    if (!audiobuffer)
        return false;

    // TODO:
    // Streaming.

    ov_callbacks ovCallbacks;
    ovCallbacks.close_func = closeOgg;
    ovCallbacks.read_func = readOgg;
    ovCallbacks.seek_func = seekOgg;
    ovCallbacks.tell_func = tellOgg;

    OggVorbis_File oggVorbisFile;

    if (ov_open_callbacks(file.get(), &oggVorbisFile, NULL, -1, ovCallbacks) < 0)
    {
        base::glog << "Failed to open ogg file" << resource->getFilePath() << base::logwarn;
        return false;
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

    if (totalGot > 0)
    {
        alBufferData(audiobuffer->getALId(), (ovInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, audioData, totalGot, ovInfo->rate);
        printOpenALError();
    }

    delete [] audioData;

    ov_clear(&oggVorbisFile);

    return true;
}

} }