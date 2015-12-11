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
    struct ComponentData
    {
        unsigned audioSourceId = 0;
        float pitch = 1.0f;
        float gain = 1.0f;
        bool looped = false;
    };

    std::vector<ComponentData> m_components;
    std::unordered_map<Entity::IdType, ComponentInstance> m_lookup;

    ComponentInstance get(Entity e) const;

    void update(float systemDelta, float gameDelta);

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
    // TODO: contains?
};

// Case 1:
// auto inst = svc().world().audio().lookup(e)
// svc().world().audio().setPitch(inst, 43.0f);
// svc().world().audio().play(inst);

}
