#include "AudioComponentProcessor.h"

#include "AudioBuffer.h"
#include "OpenALCommon.h"
#include <df3d/game/ComponentDataHolder.h>
#include <df3d/engine/3d/SceneGraphComponentProcessor.h>
#include <df3d/game/World.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceFactory.h>
#include <df3d/lib/Utils.h>

namespace df3d {

static AudioComponentProcessor::State getAudioState(unsigned audioSourceId)
{
    DF3D_ASSERT_MESS(audioSourceId, "failed to set an audio state for invalid source");

    ALint state;
    alGetSourcei(audioSourceId, AL_SOURCE_STATE, &state);

    switch (state)
    {
    case AL_PLAYING:
        return AudioComponentProcessor::State::PLAYING;
    case AL_PAUSED:
        return AudioComponentProcessor::State::PAUSED;
    case AL_STOPPED:
        return AudioComponentProcessor::State::STOPPED;
    default:
        return AudioComponentProcessor::State::INITIAL;
    }

    return AudioComponentProcessor::State::INITIAL;
}

struct AudioComponentProcessor::Impl
{
    struct Data
    {
        Entity holder;
        glm::vec3 holderPos;

        ALuint audioSourceId = 0;
        float pitch = 1.0f;
        float gain = 1.0f;
        bool looped = false;

        shared_ptr<AudioBuffer> buffer;
    };

    ComponentDataHolder<Data> data;

    float soundVolume = 1.0f;
    std::list<Data> streamingData;
    unique_ptr<std::mutex> streamingMutex;
    unique_ptr<std::thread> streamingThread;
    bool streamingThreadActive = true;

    float calcResultGain(float gain)
    {
        return soundVolume * gain;
    }
};

void AudioComponentProcessor::streamThread()
{
    /*
    while (m_pimpl->streamingThreadActive)
    {
        m_pimpl->streamingMutex->lock();

        for (auto &data : m_pimpl->streamingData)
        {
            if (data.buffer && getAudioState(data.audioSourceId) == State::PLAYING)
                data.buffer->streamData(data.audioSourceId, data.looped);
        }

        m_pimpl->streamingMutex->unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    */
}

void AudioComponentProcessor::update()
{
    // Update the transform component.
    for (auto &compData : m_pimpl->data.rawData())
        compData.holderPos = m_world->sceneGraph().getWorldPosition(compData.holder);

    /*
    for (auto &compData : m_pimpl->data.rawData())
    {
        // If it has stopped, then do not update it.
        if (getAudioState(compData.audioSourceId) == State::STOPPED)
            continue;

        alSourcefv(compData.audioSourceId, AL_POSITION, glm::value_ptr(compData.holderPos));
    }
    */
}

void AudioComponentProcessor::cleanStep(const std::list<Entity> &deleted)
{
    m_pimpl->data.cleanStep(deleted);
}

AudioComponentProcessor::AudioComponentProcessor(World *world)
    : m_pimpl(new Impl()),
    m_world(world)
{
    /*
    m_pimpl->data.setDestructionCallback([this](const Impl::Data &data) {
        if (data.buffer && data.buffer->isStreamed())
        {
            m_pimpl->streamingMutex->lock();

            for (auto it = m_pimpl->streamingData.begin(); it != m_pimpl->streamingData.end(); it++)
            {
                if (it->audioSourceId == data.audioSourceId)
                {
                    m_pimpl->streamingData.erase(it);
                    break;
                }
                else
                    it++;
            }

            m_pimpl->streamingMutex->unlock();
        }

        alDeleteSources(1, &data.audioSourceId);
    });
    */
}

AudioComponentProcessor::~AudioComponentProcessor()
{
    m_pimpl->data.clear();

    /*
    if (m_pimpl->streamingThread)
    {
        m_pimpl->streamingThreadActive = false;
        m_pimpl->streamingThread->join();
        m_pimpl->streamingThread.reset();
    }*/
}

void AudioComponentProcessor::play(Entity e)
{
    /*
    const auto &compData = m_pimpl->data.getData(e);

    if (getAudioState(compData.audioSourceId) != AudioComponentProcessor::State::PLAYING)
        alSourcePlay(compData.audioSourceId);
        */
}

void AudioComponentProcessor::stop(Entity e)
{
    /*
    const auto &compData = m_pimpl->data.getData(e);

    if (getAudioState(compData.audioSourceId) != AudioComponentProcessor::State::STOPPED)
        alSourceStop(compData.audioSourceId);
        */
}

void AudioComponentProcessor::pause(Entity e)
{
    /*
    const auto &compData = m_pimpl->data.getData(e);

    if (getAudioState(compData.audioSourceId) != AudioComponentProcessor::State::PAUSED)
        alSourcePause(compData.audioSourceId);
        */
}

void AudioComponentProcessor::setSoundVolume(float volume)
{
    m_pimpl->soundVolume = utils::clamp(volume, 0.0f, 1.0f);

    /*
    for (const auto &data : m_pimpl->data.rawData())
        alSourcef(data.audioSourceId, AL_GAIN, m_pimpl->calcResultGain(data.gain));
        */
}

void AudioComponentProcessor::setListenerPosition(const glm::vec3 &pos)
{
    /*
    alListenerfv(AL_POSITION, glm::value_ptr(pos));
    */
}

void AudioComponentProcessor::setListenerOrientation(const glm::vec3 &dir, const glm::vec3 &up)
{
    /*
    ALfloat listenerOrientation[] = { dir.x, dir.y, dir.z, up.x, up.y, up.z };
    alListenerfv(AL_ORIENTATION, listenerOrientation);
    */
}

void AudioComponentProcessor::setPitch(Entity e, float pitch)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.pitch = pitch;
    /*
    alSourcef(compData.audioSourceId, AL_PITCH, pitch);
    */
}

void AudioComponentProcessor::setGain(Entity e, float gain)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.gain = gain;
    /*
    alSourcef(compData.audioSourceId, AL_GAIN, m_pimpl->calcResultGain(gain));
    */
}

void AudioComponentProcessor::setLooped(Entity e, bool looped)
{
    auto &compData = m_pimpl->data.getData(e);
    compData.looped = looped;

    /*
    if (!compData.buffer->isStreamed())
        alSourcei(compData.audioSourceId, AL_LOOPING, looped);

    if (m_pimpl->streamingThread)
    {
        m_pimpl->streamingMutex->lock();

        auto foundStreamed = std::find_if(m_pimpl->streamingData.begin(), m_pimpl->streamingData.end(),
                                          [&compData](const Impl::Data &data) { return data.buffer == compData.buffer; });
        if (foundStreamed != m_pimpl->streamingData.end())
            foundStreamed->looped = looped;

        m_pimpl->streamingMutex->unlock();
    }
    */
}

void AudioComponentProcessor::setRolloffFactor(Entity e, float factor)
{
    auto &compData = m_pimpl->data.getData(e);
    /*
    alSourcef(compData.audioSourceId, AL_ROLLOFF_FACTOR, factor);
    */
}

float AudioComponentProcessor::getPitch(Entity e) const
{
    return m_pimpl->data.getData(e).pitch;
}

float AudioComponentProcessor::getGain(Entity e) const
{
    return m_pimpl->data.getData(e).gain;
}

bool AudioComponentProcessor::isLooped(Entity e) const
{
    return m_pimpl->data.getData(e).looped;
}

AudioComponentProcessor::State AudioComponentProcessor::getState(Entity e) const
{
    return AudioComponentProcessor::State::STOPPED;
    /*
    return getAudioState(m_pimpl->data.getData(e).audioSourceId);
    */
}

void AudioComponentProcessor::add(Entity e, const std::string &audioFilePath, bool streamed)
{
    if (m_pimpl->data.contains(e))
    {
        DFLOG_WARN("An entity already has an audio component");
        return;
    }

    Impl::Data data;
    /*
    alGenSources(1, &data.audioSourceId);
    if (!data.audioSourceId)
    {
        DFLOG_WARN("Failed to create audio source");
        return;
    }

    alSourcef(data.audioSourceId, AL_PITCH, data.pitch);
    alSourcef(data.audioSourceId, AL_GAIN, data.gain);

    auto buffer = svc().resourceManager().getFactory().createAudioBuffer(audioFilePath, streamed);
    if (buffer && buffer->isInitialized())
        buffer->attachToSource(data.audioSourceId);
    else
        DFLOG_WARN("Can not add a buffer to an audio source. Audio path: %s", audioFilePath.c_str());

    printOpenALError();
    */

    data.holder = e;
    data.holderPos = m_world->sceneGraph().getWorldPosition(e);
    /*
    data.buffer = buffer;
    */

    m_pimpl->data.add(e, data);

    /*
    if (streamed)
    {
        if (!m_pimpl->streamingThread)
        {
            m_pimpl->streamingMutex.reset(new std::mutex());
            m_pimpl->streamingThread.reset(new std::thread([this]() { streamThread(); }));
        }

        m_pimpl->streamingMutex->lock();
        m_pimpl->streamingData.push_back(data);
        m_pimpl->streamingMutex->unlock();
    }
    */
}

void AudioComponentProcessor::remove(Entity e)
{
    if (!m_pimpl->data.contains(e))
    {
        DFLOG_WARN("Failed to remove audio component from an entity. Component is not attached");
        return;
    }

    m_pimpl->data.remove(e);
}

bool AudioComponentProcessor::has(Entity e)
{
    return m_pimpl->data.lookup(e).valid();
}

}
