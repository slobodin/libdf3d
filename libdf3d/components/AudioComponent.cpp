#include "df3d_pch.h"
#include "AudioComponent.h"

#include <utils/JsonHelpers.h>
#include <base/Service.h>
#include <scene/Node.h>
#include <components/TransformComponent.h>
#include <audio/AudioBuffer.h>
#include <audio/OpenALCommon.h>

namespace df3d { namespace components {

void AudioComponent::onUpdate(float dt)
{
    if (!m_audioSourceId)
        return;

    if (m_holder->transform())
    {
        auto pos = m_holder->transform()->getPosition();
        alSourcefv(m_audioSourceId, AL_POSITION, glm::value_ptr(pos));
    }

    // If it has stopped, then remove from the holder.
    if (getState() == State::STOPPED)
    {
        m_holder->detachComponent(AUDIO);
    }
}

AudioComponent::AudioComponent(const std::string &audioFilePath)
    : NodeComponent(AUDIO)
{
    m_buffer = gsvc().resourceMgr.getFactory().createAudioBuffer(audioFilePath);
    if (!m_buffer || !m_buffer->isInitialized())
    {
        base::glog << "Can not initialize audio component from" << audioFilePath << base::logwarn;
        return;
    }

    ALuint sourceId = 0;
    alGenSources(1, &sourceId);

    alSourcef(sourceId, AL_PITCH, 1.0f);
    alSourcef(sourceId, AL_GAIN, 1.0f);
    alSourcei(sourceId, AL_BUFFER, m_buffer->getALId());

    m_audioSourceId = sourceId;

    printOpenALError();
}

AudioComponent::~AudioComponent()
{
    if (m_audioSourceId)
    {
        alDeleteSources(1, &m_audioSourceId);
        m_audioSourceId = 0;
    }
}

void AudioComponent::play()
{
    if (m_audioSourceId && getState() != State::PLAYING)
        alSourcePlay(m_audioSourceId);
}

void AudioComponent::stop()
{
    if (m_audioSourceId && getState() != State::STOPPED)
        alSourceStop(m_audioSourceId);
}

void AudioComponent::pause()
{
    if (m_audioSourceId && getState() != State::PAUSED)
        alSourcePause(m_audioSourceId);
}

void AudioComponent::setPitch(float pitch)
{
    m_pitch = pitch;
    if (m_audioSourceId)
        alSourcef(m_audioSourceId, AL_PITCH, m_pitch);
}

void AudioComponent::setGain(float gain)
{
    m_gain = gain;
    if (m_audioSourceId)
        alSourcef(m_audioSourceId, AL_GAIN, m_gain);
}

void AudioComponent::setLooped(bool looped)
{
    m_looped = looped;
    if (m_audioSourceId)
        alSourcei(m_audioSourceId, AL_LOOPING, m_looped);
}

AudioComponent::State AudioComponent::getState()
{
    if (!m_audioSourceId)
        return State::INITIAL;

    ALint state;
    alGetSourcei(m_audioSourceId, AL_SOURCE_STATE, &state);

    switch (state)
    {
    case AL_PLAYING:
        return State::PLAYING;
    case AL_PAUSED:
        return State::PAUSED;
    case AL_STOPPED:
        return State::STOPPED;
    default:
        return State::INITIAL;
    }

    return State::INITIAL;
}

shared_ptr<NodeComponent> AudioComponent::clone() const
{
    // TODO:
    assert(false);

    return nullptr;
}

} }
