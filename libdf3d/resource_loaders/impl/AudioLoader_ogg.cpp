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
        res = file->seek(static_cast<int32_t>(offset), std::ios_base::beg);
        break;
    case SEEK_CUR:
        res = file->seek(static_cast<int32_t>(offset), std::ios_base::cur);
        break;
    case SEEK_END:
        res = file->seek(static_cast<int32_t>(offset), std::ios_base::end);
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

static unique_ptr<OggVorbis_File> CreateVorbisFile(shared_ptr<FileDataSource> source)
{
    ov_callbacks ovCallbacks;
    ovCallbacks.close_func = closeOgg;
    ovCallbacks.read_func = readOgg;
    ovCallbacks.seek_func = seekOgg;
    ovCallbacks.tell_func = tellOgg;

    auto oggVorbisFile = make_unique<OggVorbis_File>();

    if (ov_open_callbacks(source.get(), oggVorbisFile.get(), NULL, -1, ovCallbacks) < 0)
    {
        DFLOG_WARN("Failed to open ogg file %s", source->getPath().c_str());
        return nullptr;
    }
    return oggVorbisFile;
}

static bool ReadOggBlock(int32_t size, char *buffer, OggVorbis_File *oggVorbisFile)
{
    // Read PCM.
    int32_t totalGot = 0;
    int32_t got;
    int currentSection;

    while (totalGot < size)
    {
        got = ov_read(oggVorbisFile, buffer + totalGot, size - totalGot, 0, 2, 1, &currentSection);

        if (got == 0)
        {
            // Done with loading.
            break;
        }
        else if (got > 0)
        {
            totalGot += got;
        }
        else
        {
            DFLOG_WARN("Failed to read ogg data");
            return false;
        }
    }

    return true;
}

class AudioStream_Ogg : public IAudioStream
{
    unique_ptr<OggVorbis_File> m_oggVorbisFile;
    shared_ptr<FileDataSource> m_source;

    size_t m_sampleRate;
    ALuint m_format;

public:
    AudioStream_Ogg(shared_ptr<FileDataSource> source, unique_ptr<OggVorbis_File> oggFile)
        : m_oggVorbisFile(std::move(oggFile)),
        m_source(std::move(source))
    {
        auto ovInfo = ov_info(m_oggVorbisFile.get(), -1);

        m_sampleRate = ovInfo->rate;
        m_format = ovInfo->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    }

    ~AudioStream_Ogg()
    {
        ov_clear(m_oggVorbisFile.get());
    }

    bool streamData(ALuint alBuffer, int32_t dataSize, char *buffer, bool looped) override
    {
        int32_t totalGot = 0;
        int32_t got;
        int currentSection;

        while (totalGot < dataSize)
        {
            got = ov_read(m_oggVorbisFile.get(), buffer + totalGot, dataSize - totalGot, 0, 2, 1, &currentSection);
            if (got > 0)
            {
                totalGot += got;
            }
            else
            {
                if (looped)
                    ov_pcm_seek(m_oggVorbisFile.get(), 0);
                break;
            }
        }

        if (totalGot > 0)
            alBufferData(alBuffer, m_format, buffer, totalGot, m_sampleRate);

        return (totalGot > 0) || looped;
    }
};

unique_ptr<PCMData> AudioLoader_ogg::load(shared_ptr<FileDataSource> source)
{
    auto oggVorbisFile = CreateVorbisFile(source);
    if (!oggVorbisFile)
        return nullptr;

    auto result = make_unique<PCMData>();

    auto ovInfo = ov_info(oggVorbisFile.get(), -1);
    int32_t pcmSize = ov_pcm_total(oggVorbisFile.get(), -1) * ovInfo->channels * 2;

    result->totalSize = pcmSize;
    result->sampleRate = ovInfo->rate;
    result->format = ovInfo->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    result->data = new char[pcmSize];

    if (!ReadOggBlock(pcmSize, result->data, oggVorbisFile.get()))
        result.reset();

    ov_clear(oggVorbisFile.get());

    return result;
}

unique_ptr<IAudioStream> AudioLoader_ogg::loadStreamed(shared_ptr<FileDataSource> source)
{
    auto oggVorbisFile = CreateVorbisFile(source);
    if (!oggVorbisFile)
        return nullptr;

    return make_unique<AudioStream_Ogg>(source, std::move(oggVorbisFile));
}

} }
