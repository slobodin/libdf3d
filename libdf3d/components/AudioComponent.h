#pragma once

#include <components/NodeComponent.h>

FWD_MODULE_CLASS(audio, AudioBuffer)

namespace df3d { namespace components {

class DF3D_DLL AudioComponent : public components::NodeComponent
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
    unsigned m_audioSourceId = 0;
    shared_ptr<audio::AudioBuffer> m_buffer;

    float m_pitch = 1.0f;
    float m_gain = 1.0f;
    bool m_looped = false;

    void onUpdate(float dt) override;

public:
    AudioComponent(const char *audioFilePath);
    AudioComponent(const Json::Value &root);
    ~AudioComponent();

    void play();
    void stop();
    void pause();

    void setPitch(float pitch);
    void setGain(float gain);
    void setLooped(bool looped);

    float getPitch() const { return m_pitch; }
    float getGain() const { return m_gain; }
    bool isLooped() const { return m_looped; }

    State getState();

    shared_ptr<NodeComponent> clone() const override;
};

} }