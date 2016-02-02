#include "AudioLoaders.h"

#include <libdf3d/audio/impl/OpenALCommon.h>
#include <libdf3d/base/EngineController.h>
#include <libdf3d/io/FileSystem.h>
#include <libdf3d/io/FileDataSource.h>
#include "impl/AudioLoader_wav.h"
#include "impl/AudioLoader_ogg.h"

namespace df3d {

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

AudioBuffer* AudioBufferFSLoader::createDummy()
{
    return new AudioBuffer();
}

bool AudioBufferFSLoader::decode(shared_ptr<FileDataSource> source)
{
    auto extension = svc().fileSystem().getFileExtension(source->getPath());

    if (extension == ".wav")
        m_pcmData = resource_loaders_impl::AudioLoader_wav().load(source);
    else if (extension == ".ogg")
        m_pcmData = resource_loaders_impl::AudioLoader_ogg().load(source);

    return m_pcmData != nullptr;
}

void AudioBufferFSLoader::onDecoded(Resource *resource)
{
    auto audioBuffer = static_cast<AudioBuffer*>(resource);

    alBufferData(audioBuffer->getALId(), convertALFormat(m_pcmData->format), m_pcmData->data, m_pcmData->dataSize, m_pcmData->sampleRate);

    // Explicitly remove PCM buffer in order to prevent caching.
    m_pcmData.reset();

    printOpenALError();
}

}
