#pragma once

#include <libdf3d/resources/Resource.h>
#include <libdf3d/audio/AudioBuffer.h>

namespace df3d {

class AudioBufferFSLoader : public FSResourceLoader
{
    std::string m_path;
    bool m_streamed;

    // Either full PCM data or stream handle will be gotten.
    unique_ptr<PCMData> m_pcmData;
    unique_ptr<IAudioStream> m_stream;

public:
    AudioBufferFSLoader(const std::string &path, bool streamed);

    AudioBuffer* createDummy() override;
    bool decode(shared_ptr<FileDataSource> source) override;
    void onDecoded(Resource *resource) override;
};

}
