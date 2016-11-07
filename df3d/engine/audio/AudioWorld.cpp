#include "AudioWorld.h"

#include "OpenALCommon.h"
#include <df3d/game/ComponentDataHolder.h>
#include <df3d/engine/3d/SceneGraphComponentProcessor.h>
#include <df3d/game/World.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/AudioResource.h>
#include <df3d/lib/Utils.h>

namespace df3d {

static AudioWorld::State GetAudioState(ALuint audioSourceId)
{
    DF3D_ASSERT_MESS(audioSourceId != 0, "Failed to set an audio state for invalid source.");

    ALint state;
    alGetSourcei(audioSourceId, AL_SOURCE_STATE, &state);

    switch (state)
    {
    case AL_PLAYING:
        return AudioWorld::State::PLAYING;
    case AL_PAUSED:
        return AudioWorld::State::PAUSED;
    case AL_STOPPED:
        return AudioWorld::State::STOPPED;
    default:
        break;
    }

    return AudioWorld::State::INITIAL;
}

const AudioWorld::AudioSource* AudioWorld::lookupSource(AudioSourceHandle handle) const
{
    return const_cast<AudioWorld*>(this)->lookupSource(handle);
}

AudioWorld::AudioSource* AudioWorld::lookupSource(AudioSourceHandle handle)
{
    if (!handle.isValid())
        return nullptr;

    auto found = m_lookup.find(handle);
    if (found == m_lookup.end())
        return nullptr;
    return &found->second;
}

void AudioWorld::streamThread()
{
    while (m_streamingThreadActive)
    {
        m_streamingMutex.lock();

        for (auto &data : m_streamingData)
        {
            if (data.audioResource && GetAudioState(data.sourceId) == State::PLAYING)
                data.audioResource->streamData(data.sourceId, data.looped);
        }

        m_streamingMutex.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

AudioWorld::AudioWorld()
    : m_handleBag(MemoryManager::allocDefault()),
    m_streamingThreadActive(true)
{
    m_streamingThread = std::thread{ [this]() { streamThread(); } };
}

AudioWorld::~AudioWorld()
{
    suspend();

    if (!m_lookup.empty())
        DFLOG_WARN("Not all audio sources have been destroyed, %d left", m_lookup.size());

    while (!m_lookup.empty())
        destroy(m_lookup.begin()->first);
}

void AudioWorld::update()
{
    printOpenALError();
}

void AudioWorld::suspend()
{
    if (!m_suspended)
    {
        if (m_streamingThreadActive)
        {
            m_streamingThreadActive = false;
            m_streamingThread.join();
            m_streamingThread = {};
        }

        DF3D_ASSERT(m_suspendedSources.empty());
        for (const auto &source : m_lookup)
        {
            if (GetAudioState(source.second.audioSourceId) == State::PLAYING)
            {
                pause(source.first);
                m_suspendedSources.push_back(source.first);
            }
        }

        m_suspended = true;
    }
}

void AudioWorld::resume()
{
    if (m_suspended)
    {
        if (!m_streamingThreadActive)
        {
            m_streamingThreadActive = true;
            m_streamingThread = std::thread{ [this]() { streamThread(); } };
        }

        for (auto source : m_suspendedSources)
            play(source);

        m_suspendedSources.clear();

        m_suspended = false;
    }
}

void AudioWorld::play(AudioSourceHandle handle)
{
    if (auto source = lookupSource(handle))
    {
        if (GetAudioState(source->audioSourceId) != State::PLAYING)
            alSourcePlay(source->audioSourceId);
    }
}

void AudioWorld::stop(AudioSourceHandle handle)
{
    if (auto source = lookupSource(handle))
    {
        if (GetAudioState(source->audioSourceId) != State::STOPPED)
            alSourceStop(source->audioSourceId);
    }
}

void AudioWorld::pause(AudioSourceHandle handle)
{
    if (auto source = lookupSource(handle))
    {
        if (GetAudioState(source->audioSourceId) != State::PAUSED)
            alSourcePause(source->audioSourceId);
    }
}

void AudioWorld::setSoundVolume(float volume)
{
    m_soundVolume = utils::clamp(volume, 0.0f, 1.0f);
    alListenerf(AL_GAIN, volume);
}

void AudioWorld::setListenerPosition(const glm::vec3 &pos)
{
    alListenerfv(AL_POSITION, glm::value_ptr(pos));
}

void AudioWorld::setListenerOrientation(const glm::vec3 &dir, const glm::vec3 &up)
{
    ALfloat listenerOrientation[] = { dir.x, dir.y, dir.z, up.x, up.y, up.z };
    alListenerfv(AL_ORIENTATION, listenerOrientation);
}

void AudioWorld::setListenerVelocity(const glm::vec3 &velocity)
{
    alListenerfv(AL_VELOCITY, glm::value_ptr(velocity));
}

void AudioWorld::setPitch(AudioSourceHandle handle, float pitch)
{
    if (auto source = lookupSource(handle))
    {
        source->pitch = pitch;
        alSourcef(source->audioSourceId, AL_PITCH, pitch);
    }
}

void AudioWorld::setGain(AudioSourceHandle handle, float gain)
{
    if (auto source = lookupSource(handle))
    {
        source->gain = gain;
        alSourcef(source->audioSourceId, AL_GAIN, gain);
    }
}

void AudioWorld::setLooped(AudioSourceHandle handle, bool looped)
{
    if (auto source = lookupSource(handle))
    {
        source->looped = looped;
        if (!source->audioResource->isStreamed())
            alSourcei(source->audioSourceId, AL_LOOPING, looped);

        m_streamingMutex.lock();

        auto foundStreamed = std::find_if(m_streamingData.begin(), m_streamingData.end(),
                                          [source](const StreamingData &data) { return data.audioResource == source->audioResource; });
        if (foundStreamed != m_streamingData.end())
            foundStreamed->looped = looped;

        m_streamingMutex.unlock();
    }
}

void AudioWorld::setRolloffFactor(AudioSourceHandle handle, float factor)
{
    if (auto source = lookupSource(handle))
        alSourcef(source->audioSourceId, AL_ROLLOFF_FACTOR, factor);
}

void AudioWorld::setPosition(AudioSourceHandle handle, const glm::vec3 &pos)
{
    if (auto source = lookupSource(handle))
        alSourcefv(source->audioSourceId, AL_POSITION, glm::value_ptr(pos));
}

void AudioWorld::setVelocity(AudioSourceHandle handle, const glm::vec3 &velocity)
{
    if (auto source = lookupSource(handle))
        alSourcefv(source->audioSourceId, AL_VELOCITY, glm::value_ptr(velocity));
}

float AudioWorld::getPitch(AudioSourceHandle handle) const
{
    if (auto source = lookupSource(handle))
        return source->pitch;
    return 0.0f;
}

float AudioWorld::getGain(AudioSourceHandle handle) const
{
    if (auto source = lookupSource(handle))
        return source->gain;
    return 0.0f;
}

bool AudioWorld::isLooped(AudioSourceHandle handle) const
{
    if (auto source = lookupSource(handle))
        return source->looped;
    return false;
}

AudioWorld::State AudioWorld::getState(AudioSourceHandle handle) const
{
    auto audioSource = lookupSource(handle);
    if (!audioSource)
        return State::INITIAL;

    return GetAudioState(audioSource->audioSourceId);
}

ResourceID AudioWorld::getResourceId(AudioSourceHandle handle) const
{
    auto audioSource = lookupSource(handle);
    if (!audioSource)
        return{};
    return audioSource->resourceId;
}

AudioSourceHandle AudioWorld::create(const std::string &audioFilePath, bool looped)
{
    AudioSource source;

    alGenSources(1, &source.audioSourceId);
    if (!source.audioSourceId)
    {
        DFLOG_WARN("Failed to create audio source");
        return {};
    }

    auto handle = AudioSourceHandle(m_handleBag.getNew());

    DF3D_ASSERT(!utils::contains_key(m_lookup, handle));

    auto audioResource = svc().resourceManager().getResource<AudioResource>(audioFilePath);
    if (audioResource)
    {
        source.gain = audioResource->gain;
        alSourcef(source.audioSourceId, AL_ROLLOFF_FACTOR, audioResource->rolloff);
        audioResource->attachToSource(source.audioSourceId);
    }
    else
        DFLOG_WARN("Can not add a buffer to an audio source. Audio path: %s", audioFilePath.c_str());

    alSourcef(source.audioSourceId, AL_PITCH, source.pitch);
    alSourcef(source.audioSourceId, AL_GAIN, source.gain);

    source.audioResource = audioResource;
    source.looped = looped;
    source.resourceId = audioFilePath;

    printOpenALError();

    m_lookup[handle] = source;

    if (audioResource && audioResource->isStreamed())
    {
        StreamingData streamingData;
        streamingData.audioResource = audioResource;
        streamingData.looped = looped;
        streamingData.sourceId = source.audioSourceId;

        m_streamingMutex.lock();
        m_streamingData.push_back(streamingData);
        m_streamingMutex.unlock();
    }
    else
        alSourcei(source.audioSourceId, AL_LOOPING, looped);

    return handle;
}

void AudioWorld::destroy(AudioSourceHandle handle)
{
    auto found = m_lookup.find(handle);

    if (found != m_lookup.end())
    {
        if (found->second.audioResource && found->second.audioResource->isStreamed())
        {
            m_streamingMutex.lock();

            for (auto it = m_streamingData.begin(); it != m_streamingData.end(); )
            {
                if (it->sourceId == found->second.audioSourceId)
                {
                    m_streamingData.erase(it);
                    break;
                }
                else
                    it++;
            }

            m_streamingMutex.unlock();
        }

        alDeleteSources(1, &found->second.audioSourceId);

        m_lookup.erase(found);
        m_handleBag.release(handle.getID());
    }
    else
        DFLOG_WARN("Failed to destroy an audio source. Source is not exists");
}

}
