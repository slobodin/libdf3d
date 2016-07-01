#include "AudioLoaders.h"

#include <df3d/engine/audio/OpenALCommon.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/io/DefaultFileSystem.h>
#include <df3d/engine/io/FileSystemHelpers.h>
#include <df3d/engine/io/DataSource.h>
#include "loaders/AudioLoader_wav.h"
#include "loaders/AudioLoader_ogg.h"

namespace df3d {

AudioBufferFSLoader::AudioBufferFSLoader(const std::string &path, bool streamed)
    : FSResourceLoader(ResourceLoadingMode::IMMEDIATE),
    m_path(path),
    m_streamed(streamed)
{

}

AudioBuffer* AudioBufferFSLoader::createDummy()
{
    return new AudioBuffer(m_streamed);
}

bool AudioBufferFSLoader::decode(shared_ptr<DataSource> source)
{
    const auto extension = FileSystemHelpers::getFileExtension(source->getPath());

    if (extension == ".wav")
    {
        DF3D_ASSERT_MESS(!m_streamed, "streaming for wav is unsupported for now");   // FIXME:
        m_pcmData = resource_loaders_impl::AudioLoader_wav().load(source);
        return m_pcmData != nullptr;
    }
    else if (extension == ".ogg")
    {
        if (m_streamed)
        {
            m_stream = resource_loaders_impl::AudioLoader_ogg().loadStreamed(source);
            return m_stream != nullptr;
        }
        else
        {
            m_pcmData = resource_loaders_impl::AudioLoader_ogg().load(source);
            return m_pcmData != nullptr;
        }
    }

    return false;
}

void AudioBufferFSLoader::onDecoded(Resource *resource)
{
    auto audioBuffer = static_cast<AudioBuffer*>(resource);

    if (m_pcmData)
        audioBuffer->init(std::move(m_pcmData));
    else
        audioBuffer->init(std::move(m_stream));
}

}
