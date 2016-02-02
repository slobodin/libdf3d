#pragma once

#include <libdf3d/resources/Resource.h>
#include <libdf3d/audio/AudioBuffer.h>

namespace df3d {

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

    AudioBuffer* createDummy() override;
    bool decode(shared_ptr<FileDataSource> source) override;
    void onDecoded(Resource *resource) override;
};

}
