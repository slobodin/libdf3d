#include "df3d_pch.h"
#include "AudioComponentProcessor.h"

#include "AudioBuffer.h"
#include "impl/OpenALCommon.h"
#include <base/EngineController.h>
#include <resources/ResourceManager.h>
#include <resources/ResourceFactory.h>
#include <utils/Utils.h>

namespace df3d {

static AudioComponentProcessor::State getAudioState(unsigned audioSourceId)
{
    assert(audioSourceId);

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

    return  AudioComponentProcessor::State::INITIAL;
}

ComponentInstance AudioComponentProcessor::get(Entity e) const
{
    auto found = m_lookup.find(e.id);
    if (found == m_lookup.end())
        return ComponentInstance();

    return found->second;
}

void AudioComponentProcessor::update(float systemDelta, float gameDelta)
{
    /*
    if (!m_audioSourceId)
        return;

    if (m_holder->transform())
    {
        auto pos = m_holder->transform()->getPosition();
        alSourcefv(m_audioSourceId, AL_POSITION, glm::value_ptr(pos));
    }

    // If it has stopped, then remove from the holder.
    if (getState() == State::STOPPED)
        m_holder->detachComponent(ComponentType::AUDIO);
    */
}

AudioComponentProcessor::AudioComponentProcessor()
{

}

AudioComponentProcessor::~AudioComponentProcessor()
{

}

void AudioComponentProcessor::play(Entity e)
{
    const auto &compData = m_components.at(get(e).id);

    if (getAudioState(compData.audioSourceId) != AudioComponentProcessor::State::PLAYING)
        alSourcePlay(compData.audioSourceId);
}

void AudioComponentProcessor::stop(Entity e)
{
    const auto &compData = m_components.at(get(e).id);

    if (getAudioState(compData.audioSourceId) != AudioComponentProcessor::State::STOPPED)
        alSourceStop(compData.audioSourceId);
}

void AudioComponentProcessor::pause(Entity e)
{
    const auto &compData = m_components.at(get(e).id);

    if (getAudioState(compData.audioSourceId) != AudioComponentProcessor::State::PAUSED)
        alSourcePause(compData.audioSourceId);
}

void AudioComponentProcessor::setPitch(Entity e, float pitch)
{
    m_components.at(get(e).id).pitch = pitch;
    alSourcef(m_components.at(get(e).id).audioSourceId, AL_PITCH, pitch);
}

void AudioComponentProcessor::setGain(Entity e, float gain)
{
    m_components.at(get(e).id).gain = gain;
    alSourcef(m_components.at(get(e).id).audioSourceId, AL_GAIN, gain);
}

void AudioComponentProcessor::setLooped(Entity e, bool looped)
{
    m_components.at(get(e).id).looped = looped;
    alSourcei(m_components.at(get(e).id).audioSourceId, AL_LOOPING, looped);
}

float AudioComponentProcessor::getPitch(Entity e) const
{
    return m_components.at(get(e).id).pitch;
}

float AudioComponentProcessor::getGain(Entity e) const
{
    return m_components.at(get(e).id).gain;
}

bool AudioComponentProcessor::isLooped(Entity e) const
{
    return m_components.at(get(e).id).looped;
}

void AudioComponentProcessor::add(Entity e, const std::string &audioFilePath)
{
    if (utils::contains_key(m_lookup, e.id))
    {
        glog << "An entity already has an audio component" << logwarn;
        return;
    }

    auto buffer = svc().resourceManager().getFactory().createAudioBuffer(audioFilePath);
    if (!buffer || !buffer->isInitialized())
    {
        glog << "Can not add audio component to an entity. Audio path:" << audioFilePath << logwarn;
        return;
    }

    ComponentData data;

    alGenSources(1, &data.audioSourceId);
    alSourcef(data.audioSourceId, AL_PITCH, data.pitch);
    alSourcef(data.audioSourceId, AL_GAIN, data.gain);
    alSourcei(data.audioSourceId, AL_BUFFER, buffer->getALId());

    printOpenALError();
    if (!data.audioSourceId)
    {
        glog << "Failed to create audio source" << logwarn;
        return;
    }

    ComponentInstance inst(m_components.size());
    m_components.push_back(data);
    m_lookup[e.id] = inst;
}

void AudioComponentProcessor::remove(Entity e)
{
    if (!utils::contains_key(m_lookup, e.id))
    {
        glog << "Failed to remove audio component from an entity. Component is not attached" << logwarn;
        return;
    }

    const auto &data = m_components.at(get(e).id);
    alDeleteSources(1, &data.audioSourceId);

    // TODO_ecs
    assert(false);
}

}
