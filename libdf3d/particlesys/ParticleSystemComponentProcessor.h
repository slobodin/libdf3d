#pragma once

#include <scene/Entity.h>
#include <scene/EntityComponentProcessor.h>
#include <SPARK.h>

namespace df3d {

class DF3D_DLL ParticleSystemComponentProcessor : public EntityComponentProcessor
{
    struct Impl;
    unique_ptr<Impl> m_pimpl;

    void update(float systemDelta, float gameDelta) override;
    void draw(RenderQueue *ops) override;

public:
    ParticleSystemComponentProcessor();
    ~ParticleSystemComponentProcessor();

    void stop(ComponentInstance comp);
    void pause(ComponentInstance comp, bool paused);
    void setSystemLifeTime(ComponentInstance comp, float lifeTime);
    void setWorldTransformed(ComponentInstance comp, bool worldTransformed);

    float getSystemLifeTime(ComponentInstance comp) const;

    ComponentInstance add(Entity e, const std::string &vfxResource);
    void remove(Entity e);
    ComponentInstance lookup(Entity e);
};

}
