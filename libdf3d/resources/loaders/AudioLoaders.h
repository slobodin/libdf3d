#pragma once

#include <resources/Resource.h>
#include <audio/AudioBuffer.h>

namespace df3d { namespace resources {

class AudioBufferFSLoader : public FSResourceLoader
{
public:
    struct PCMData
    {
        enum class Format
        {
            MONO_8,
            MONO_16,
            STEREO_8,
            STEREO_16
        };

        char *data = nullptr;
        size_t dataSize = 0;
        size_t sampleRate = 0;
        Format format;

        ~PCMData()
        {
            delete [] data;
        }
    };

private:
    unique_ptr<PCMData> m_pcmData;
    std::string m_path;

public:
    AudioBufferFSLoader(const std::string &path);

    audio::AudioBuffer* createDummy(const ResourceGUID &guid) override;
    void decode(shared_ptr<FileDataSource> source) override;
    void onDecoded(Resource *resource) override;
};

} }
