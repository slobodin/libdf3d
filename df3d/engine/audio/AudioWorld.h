#pragma once

#include <df3d/lib/Handles.h>

namespace df3d {

class AudioBuffer;

DF3D_MAKE_HANDLE(AudioSourceHandle)

class DF3D_DLL AudioWorld : NonCopyable
{
public:
    enum class State
    {
        INITIAL,
        PLAYING,
        PAUSED,
        STOPPED
    };

private:
    struct AudioSource
    {
        unsigned int audioSourceId = 0;
        float pitch = 1.0f;
        float gain = 1.0f;
        bool looped = false;

        shared_ptr<AudioBuffer> buffer;
    };

    struct StreamingData
    {
        shared_ptr<AudioBuffer> buffer;
        unsigned int sourceId = 0;
        bool looped = false;
    };

    const AudioSource* lookupSource(AudioSourceHandle handle) const;
    AudioSource* lookupSource(AudioSourceHandle handle);

    HandleBag<AudioSourceHandle> m_handleBag;
    std::unordered_map<decltype(AudioSourceHandle::id), AudioSource> m_lookup;

    std::mutex m_streamingMutex;
    std::thread m_streamingThread;
    std::list<StreamingData> m_streamingData;

    float m_soundVolume = 1.0f;
    std::atomic<bool> m_streamingThreadActive = true;

    void streamThread();

public:
    AudioWorld();
    ~AudioWorld();

    void update();

    void play(AudioSourceHandle handle);
    void stop(AudioSourceHandle handle);
    void pause(AudioSourceHandle handle);

    void setSoundVolume(float volume);

    void setListenerPosition(const glm::vec3 &pos);
    void setListenerOrientation(const glm::vec3 &dir, const glm::vec3 &up);
    void setListenerVelocity(const glm::vec3 &velocity);

    void setPitch(AudioSourceHandle handle, float pitch);
    void setGain(AudioSourceHandle handle, float gain);
    void setLooped(AudioSourceHandle handle, bool looped);
    void setRolloffFactor(AudioSourceHandle handle, float factor);
    void setPosition(AudioSourceHandle handle, const glm::vec3 &pos);
    void setVelocity(AudioSourceHandle handle, const glm::vec3 &velocity);

    float getPitch(AudioSourceHandle handle) const;
    float getGain(AudioSourceHandle handle) const;
    bool isLooped(AudioSourceHandle handle) const;
    State getState(AudioSourceHandle handle) const;

    AudioSourceHandle create(const std::string &audioFilePath, bool streamed, bool looped);
    void destroy(AudioSourceHandle handle);
};

}
