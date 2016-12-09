#include "AudioLoader_ogg.h"

#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/audio/OpenALCommon.h>
#include <df3d/engine/resources/AudioResource.h>
#include <df3d/engine/resources/ResourceDataSource.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceFileSystem.h>

namespace df3d {

class AudioStream_Ogg : public IAudioStream
{
    unique_ptr<OggVorbis_File> m_oggVorbisFile;

    size_t m_sampleRate;
    ALuint m_format;

public:
    AudioStream_Ogg(unique_ptr<OggVorbis_File> oggFile)
        : m_oggVorbisFile(std::move(oggFile))
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

static size_t ReadOgg(void *ptr, size_t size, size_t nmemb, void *datasource)
{
    auto file = reinterpret_cast<ResourceDataSource *>(datasource);

    return file->read(ptr, size * nmemb);
}

static int SeekOgg(void *datasource, ogg_int64_t offset, int whence)
{
    auto file = reinterpret_cast<ResourceDataSource *>(datasource);

    bool res;
    switch (whence)
    {
    case SEEK_SET:
        res = file->seek(static_cast<int32_t>(offset), SeekDir::BEGIN);
        break;
    case SEEK_CUR:
        res = file->seek(static_cast<int32_t>(offset), SeekDir::CURRENT);
        break;
    case SEEK_END:
        res = file->seek(static_cast<int32_t>(offset), SeekDir::CURRENT);
        break;
    default:
        return -1;
    }

    return res ? 0 : -1;
}

static long TellOgg(void *datasource)
{
    auto file = reinterpret_cast<ResourceDataSource *>(datasource);
    return file->tell();
}

static int CloseOgg(void *datasource)
{
    auto file = reinterpret_cast<ResourceDataSource *>(datasource);
    svc().resourceManager().getFS().close(file);
    return 0;
}

static unique_ptr<OggVorbis_File> CreateVorbisFile(ResourceDataSource *source)
{
    ov_callbacks ovCallbacks;
    ovCallbacks.close_func = CloseOgg;
    ovCallbacks.read_func = ReadOgg;
    ovCallbacks.seek_func = SeekOgg;
    ovCallbacks.tell_func = TellOgg;

    auto oggVorbisFile = make_unique<OggVorbis_File>();

    if (ov_open_callbacks(source, oggVorbisFile.get(), NULL, -1, ovCallbacks) < 0)
    {
        DFLOG_WARN("Failed to open ogg file");
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

unique_ptr<PCMData> AudioLoader_ogg(const char *path, Allocator &alloc)
{
    auto source = svc().resourceManager().getFS().open(path);
    if (!source)
        return nullptr;

    auto oggVorbisFile = CreateVorbisFile(source);
    if (!oggVorbisFile)
        return nullptr;

    auto result = make_unique<PCMData>(alloc);

    auto ovInfo = ov_info(oggVorbisFile.get(), -1);
    int32_t pcmSize = ov_pcm_total(oggVorbisFile.get(), -1) * ovInfo->channels * 2;

    result->data.resize(pcmSize);
    result->sampleRate = ovInfo->rate;
    result->format = ovInfo->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    if (!ReadOggBlock(pcmSize, result->data.data(), oggVorbisFile.get()))
        result.reset();

    ov_clear(oggVorbisFile.get());

    return result;
}

unique_ptr<IAudioStream> AudioLoader_ogg_streamed(const char *path, Allocator &alloc)
{
    auto source = svc().resourceManager().getFS().open(path);
    if (!source)
        return nullptr;

    auto oggVorbisFile = CreateVorbisFile(source);
    if (!oggVorbisFile)
        return nullptr;

    return make_unique<AudioStream_Ogg>(std::move(oggVorbisFile));
}

}
