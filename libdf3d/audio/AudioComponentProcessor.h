#pragma once

#include <scene/Entity.h>

namespace df3d {

class DF3D_DLL AudioComponentProcessor : utils::NonCopyable
{
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

    ComponentInstance get(Entity e);
    ComponentInstance add(Entity e);
    void remove(Entity e);
};

}
