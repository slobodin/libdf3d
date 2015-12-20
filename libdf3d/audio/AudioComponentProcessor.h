#pragma once

#include <game/Entity.h>
#include <game/EntityComponentProcessor.h>

namespace df3d {

class AudioBuffer;
class World;

class DF3D_DLL AudioComponentProcessor : public EntityComponentProcessor
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

    void update(float systemDelta, float gameDelta) override;
    void cleanStep(World &w) override;

public:
    AudioComponentProcessor();
    ~AudioComponentProcessor();

    void play(Entity e);
    void stop(Entity e);
    void pause(Entity e);

    void setPitch(Entity e, float pitch);
    void setGain(Entity e, float gain);
    void setLooped(Entity e, bool looped);

    float getPitch(Entity e) const;
    float getGain(Entity e) const;
    bool isLooped(Entity e) const;

    void add(Entity e, const std::string &audioFilePath);
    void remove(Entity e);
};

}
