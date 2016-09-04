#pragma once

#include <df3d/game/Entity.h>
#include <df3d/game/EntityComponentProcessor.h>
#include <SPARK.h>

namespace df3d {

class RenderQueue;
class World;

struct ParticleSystemCreationParams
{
    bool worldTransformed = true;
    float systemLifeTime = -1.0f;
    SPK::Ref<SPK::System> spkSystem;

    ParticleSystemCreationParams clone() const
    {
        ParticleSystemCreationParams result;
        result.worldTransformed = worldTransformed;
        result.systemLifeTime = systemLifeTime;
        result.spkSystem = SPK::SPKObject::copy(spkSystem);
        return result;
    }
};

class DF3D_DLL ParticleSystemComponentProcessor : public EntityComponentProcessor
{
    friend class World;

    struct Impl;
    unique_ptr<Impl> m_pimpl;

    World *m_world;
    bool m_pausedGlobal = false;

    void update() override;

public:
    ParticleSystemComponentProcessor(World *world);
    ~ParticleSystemComponentProcessor();

    void useRealStep();
    void useConstantStep(float time);

    void stop(Entity e);
    void pause(Entity e, bool paused);
    void pauseGlobal(bool paused);
    void setVisible(Entity e, bool visible);
    void setSystemLifeTime(Entity e, float lifeTime);
    void setWorldTransformed(Entity e, bool worldTransformed);

    float getSystemLifeTime(Entity e) const;
    SPK::Ref<SPK::System> getSystem(Entity e) const;
    bool isWorldTransformed(Entity e) const;
    bool isPlaying(Entity e) const;
    bool isVisible(Entity e) const;

    void add(Entity e, const char *vfxResource);
    void add(Entity e, const ParticleSystemCreationParams &params);
    void remove(Entity e) override;
    bool has(Entity e) override;

    // FIXME: using this method because of optimizations & spark behaviour.
    void render();
};

}
