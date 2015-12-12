#pragma once

#include <scene/Entity.h>

namespace df3d {

class AudioBuffer;

class DF3D_DLL AudioComponentProcessor : utils::NonCopyable
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
    struct Impl;
    unique_ptr<Impl> m_pimpl;

    void update(float systemDelta, float gameDelta);

public:
    AudioComponentProcessor();
    ~AudioComponentProcessor();

    void play(ComponentInstance comp);
    void stop(ComponentInstance comp);
    void pause(ComponentInstance comp);

    void setPitch(ComponentInstance comp, float pitch);
    void setGain(ComponentInstance comp, float gain);
    void setLooped(ComponentInstance comp, bool looped);

    float getPitch(ComponentInstance comp) const;
    float getGain(ComponentInstance comp) const;
    bool isLooped(ComponentInstance comp) const;

    void add(Entity e, const std::string &audioFilePath);
    void remove(Entity e);
    ComponentInstance lookup(Entity e);
};

}
