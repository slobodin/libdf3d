#pragma once

#include <scene/Entity.h>
#include <scene/EntityComponentProcessor.h>
#include <SPARK.h>

namespace df3d {

class DF3D_DLL ParticleSystemComponentProcessor : public EntityComponentProcessor
{
    friend class World;

    struct Impl;
    unique_ptr<Impl> m_pimpl;

    void update(float systemDelta, float gameDelta) override;
    void draw(RenderQueue *ops);
    void cleanStep(World &w) override;

public:
    ParticleSystemComponentProcessor();
    ~ParticleSystemComponentProcessor();

    void stop(Entity e);
    void pause(Entity e, bool paused);
    void setSystemLifeTime(Entity e, float lifeTime);
    void setWorldTransformed(Entity e, bool worldTransformed);

    float getSystemLifeTime(Entity e) const;

    void add(Entity e, const std::string &vfxResource);
    void remove(Entity e);
};

}
