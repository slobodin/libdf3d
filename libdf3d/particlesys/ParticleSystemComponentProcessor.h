#pragma once

#include <game/Entity.h>
#include <game/EntityComponentProcessor.h>
#include <SPARK.h>

namespace df3d {

class RenderQueue;
class World;

class DF3D_DLL ParticleSystemComponentProcessor : public EntityComponentProcessor
{
    friend class World;

    struct Impl;
    unique_ptr<Impl> m_pimpl;

    World *m_world;

    void update() override;
    void draw(RenderQueue *ops);
    void cleanStep(const std::list<Entity> &deleted) override;

public:
    ParticleSystemComponentProcessor(World *world);
    ~ParticleSystemComponentProcessor();

    void stop(Entity e);
    void pause(Entity e, bool paused);
    void setSystemLifeTime(Entity e, float lifeTime);
    void setWorldTransformed(Entity e, bool worldTransformed);

    float getSystemLifeTime(Entity e) const;
    SPK::Ref<SPK::System> getSystem(Entity e) const;
    bool isWorldTransformed(Entity e) const;
    bool isPlaying(Entity e) const;

    void add(Entity e, const std::string &vfxResource);
    void add(Entity e, SPK::Ref<SPK::System> spkSystem);
    void remove(Entity e);
    bool has(Entity e);
};

}
