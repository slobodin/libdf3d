#include "AudioComponentProcessor.h"

#include "AudioBuffer.h"
#include "impl/OpenALCommon.h"
#include <libdf3d/game/ComponentDataHolder.h>
#include <libdf3d/3d/SceneGraphComponentProcessor.h>
#include <libdf3d/game/World.h>
#include <libdf3d/3d/Camera.h>
#include <libdf3d/base/EngineController.h>
#include <libdf3d/resources/ResourceManager.h>
#include <libdf3d/resources/ResourceFactory.h>
#include <libdf3d/utils/Utils.h>

namespace df3d {

static AudioComponentProcessor::State getAudioState(unsigned audioSourceId)
{
    DF3D_ASSERT(audioSourceId, "failed to set an audio state for invalid source");

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

        unsigned audioSourceId = 0;
        float pitch = 1.0f;
        float gain = 1.0f;
        bool looped = false;
    };

    ComponentDataHolder<Data> data;
};

void AudioComponentProcessor::update()
{
    // Update camera position.
    const auto &cam = *m_world->getCamera();
    alListenerfv(AL_POSITION, glm::value_ptr(cam.getPosition()));

    const auto &dir = cam.getDir();
    const auto &up = cam.getUp();

    ALfloat listenerOrientation[] = { dir.x, dir.y, dir.z, up.x, up.y, up.z };
    alListenerfv(AL_ORIENTATION, listenerOrientation);

    // Update the transform component.
    for (auto &compData : m_pimpl->data.rawData())
        compData.holderPos = m_world->sceneGraph().getWorldPosition(compData.holder);

    for (auto &compData : m_pimpl->data.rawData())
    {
        // If it has stopped, then do not update it.
        if (getAudioState(compData.audioSourceId) == State::STOPPED)
            continue;

        alSourcefv(compData.audioSourceId, AL_POSITION, glm::value_ptr(compData.holderPos));
    }
}

void AudioComponentProcessor::cleanStep(const std::list<Entity> &deleted)
{
    m_pimpl->data.cleanStep(deleted);
}

AudioComponentProcessor::AudioComponentProcessor(World *world)
    : m_pimpl(new Impl()),
    m_world(world)
{
    m_pimpl->data.setDestructionCallback([](const Impl::Data &data) {
        alDeleteSources(1, &data.audioSourceId);
    });
}

AudioComponentProcessor::~AudioComponentProcessor()
{
    //glog << "AudioComponentProcessor::~AudioComponentProcessor alive entities" << m_pimpl->data.rawData().size() << logdebug;
}

void AudioComponentProcessor::play(Entity e)
{
    const auto &compData = m_pimpl->data.getData(e);

    if (getAudioState(compData.audioSourceId) != AudioComponentProcessor::State::PLAYING)
        alSourcePlay(compData.audioSourceId);
}

void AudioComponentProcessor::stop(Entity e)
{
    const auto &compData = m_pimpl->data.getData(e);

    if (getAudioState(compData.audioSourceId) != AudioComponentProcessor::State::STOPPED)
        alSourceStop(compData.audioSourceId);
}

void AudioComponentProcessor::pause(Entity e)
{
    const auto &compData = m_pimpl->data.getData(e);

    if (getAudioState(compData.audioSourceId) != AudioComponentProcessor::State::PAUSED)
        alSourcePause(compData.audioSourceId);
}

void AudioComponentProcessor::setPitch(Entity e, float pitch)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.pitch = pitch;
    alSourcef(compData.audioSourceId, AL_PITCH, pitch);
}

void AudioComponentProcessor::setGain(Entity e, float gain)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.gain = gain;
    alSourcef(compData.audioSourceId, AL_GAIN, gain);
}

void AudioComponentProcessor::setLooped(Entity e, bool looped)
{
    auto &compData = m_pimpl->data.getData(e);
    compData.looped = looped;
    alSourcei(compData.audioSourceId, AL_LOOPING, looped);
}

void AudioComponentProcessor::setRolloffFactor(Entity e, float factor)
{
    auto &compData = m_pimpl->data.getData(e);
    alSourcef(compData.audioSourceId, AL_ROLLOFF_FACTOR, factor);
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
    return getAudioState(m_pimpl->data.getData(e).audioSourceId);
}

void AudioComponentProcessor::add(Entity e, const std::string &audioFilePath)
{
    if (m_pimpl->data.contains(e))
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

    Impl::Data data;

    alGenSources(1, &data.audioSourceId);
    if (!data.audioSourceId)
    {
        glog << "Failed to create audio source" << logwarn;
        return;
    }

    alSourcef(data.audioSourceId, AL_PITCH, data.pitch);
    alSourcef(data.audioSourceId, AL_GAIN, data.gain);
    alSourcei(data.audioSourceId, AL_BUFFER, buffer->getALId());

    printOpenALError();

    data.holder = e;
    data.holderPos = m_world->sceneGraph().getWorldPosition(e);

    m_pimpl->data.add(e, data);
}

void AudioComponentProcessor::remove(Entity e)
{
    if (!m_pimpl->data.contains(e))
    {
        glog << "Failed to remove audio component from an entity. Component is not attached" << logwarn;
        return;
    }

    m_pimpl->data.remove(e);
}

bool AudioComponentProcessor::has(Entity e)
{
    return m_pimpl->data.lookup(e).valid();
}

}
