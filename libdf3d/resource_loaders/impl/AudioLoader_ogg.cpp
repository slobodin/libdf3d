#include "AudioLoader_ogg.h"

#include <libdf3d/audio/impl/OpenALCommon.h>
#include <libdf3d/base/EngineController.h>
#include <libdf3d/io/FileSystem.h>
#include <libdf3d/io/FileDataSource.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace df3d { namespace resource_loaders_impl {

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
        res = file->seek(offset, std::ios_base::beg);
        break;
    case SEEK_CUR:
        res = file->seek(offset, std::ios_base::cur);
        break;
    case SEEK_END:
        res = file->seek(offset, std::ios_base::end);
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

unique_ptr<AudioBufferFSLoader::PCMData> AudioLoader_ogg::load(shared_ptr<FileDataSource> source)
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
        glog << "Failed to open ogg file" << source->getPath() << logwarn;
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

} }
