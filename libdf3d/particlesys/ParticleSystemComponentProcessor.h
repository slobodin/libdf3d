#pragma once

#include <scene/Entity.h>
#include <SPARK.h>

namespace df3d {

class DF3D_DLL ParticleSystemComponentProcessor : utils::NonCopyable
{
    struct Impl;
    unique_ptr<Impl> m_pimpl;

public:
    ParticleSystemComponentProcessor();
    ~ParticleSystemComponentProcessor();

    void stop(ComponentInstance comp);
    void pause(ComponentInstance comp, bool paused);
    void setSystemLifeTime(ComponentInstance comp, float lifeTime);
    void setWorldTransformed(ComponentInstance comp, bool worldTransformed);

    float getSystemLifeTime(ComponentInstance comp) const;

    ComponentInstance add(Entity e, const std::string &jsonResource);
    void remove(Entity e);
    ComponentInstance lookup(Entity e);
};

}
