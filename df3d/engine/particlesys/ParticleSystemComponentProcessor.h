#pragma once

#include <df3d/game/Entity.h>
#include <df3d/game/EntityComponentProcessor.h>
#include <df3d/game/ComponentDataHolder.h>
#include <SPARK.h>

namespace df3d {

struct RenderQueue;
class World;
class ParticleSystemBuffers_Quad;

class ParticleSystemComponentProcessor : public EntityComponentProcessor
{
    friend class World;

    struct Data
    {
        glm::mat4 holderTransform;
        SPK::Ref<SPK::System> system;
        Entity holder;
        float systemLifeTime = -1.0f;
        float systemAge = 0.0f;
        bool paused = false;
        bool visible = true;
        bool worldTransformed = true;
    };

    ComponentDataHolder<Data> m_data;
    ParticleSystemBuffers_Quad *m_quadBuffers = nullptr;

    World &m_world;
    Allocator &m_allocator;
    bool m_pausedGlobal = false;

    void updateCameraPosition(Data &compData);
    void update() override;

public:
    ParticleSystemComponentProcessor(World &world);
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

    // NOTE: duplicating particle system!
    void addWithResource(Entity e, const ResourceID &resourceID);
    void addWithSpkSystem(Entity e, SPK::Ref<SPK::System> system);
    void remove(Entity e) override;
    bool has(Entity e) override;

    // FIXME: using this method because of optimizations & spark behaviour.
    void render();
};

}
