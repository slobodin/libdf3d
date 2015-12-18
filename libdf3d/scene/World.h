#pragma once

#include "Entity.h"
#include "Camera.h"
#include "WorldRenderingParams.h"

namespace df3d {

class EntityManager;
class AudioComponentProcessor;
class StaticMeshComponentProcessor;
class ParticleSystemComponentProcessor;
class PhysicsComponentProcessor;
class TransformComponentProcessor;
class RenderQueue;
class EntityComponentProcessor;

class DF3D_DLL World : utils::NonCopyable
{
    friend class EngineController;
    friend class RenderManager;

    unique_ptr<EntityManager> m_entityManager;

    unique_ptr<EntityComponentProcessor> m_audio;
    unique_ptr<EntityComponentProcessor> m_staticMeshes;
    unique_ptr<EntityComponentProcessor> m_vfx;
    unique_ptr<EntityComponentProcessor> m_physics;
    unique_ptr<EntityComponentProcessor> m_tranform;

    Camera m_camera;
    WorldRenderingParams m_renderingParams;

    bool m_paused = false;

    void update(float systemDelta, float gameDelta);
    void collectRenderOperations(RenderQueue *ops);
    void cleanStep();

public:
    World();
    World(const std::string &worldResource);
    ~World();

    Entity spawn();
    Entity spawn(const std::string &entityResource);
    bool alive(Entity e);
    void destroy(Entity e);

    void pauseSimulation(bool paused);

    void setCamera(const Camera &camera) { m_camera = camera; }
    void setRenderingParams(const WorldRenderingParams &params) { m_renderingParams = params; }

    Camera& getCamera() { return m_camera; }
    const Camera& getCamera() const { return m_camera; }

    WorldRenderingParams& getRenderingParams() { return m_renderingParams; }
    const WorldRenderingParams& getRenderingParams() const { return m_renderingParams; }

    AudioComponentProcessor& audio();
    StaticMeshComponentProcessor& staticMesh();
    ParticleSystemComponentProcessor& vfx();
    PhysicsComponentProcessor& physics();
    TransformComponentProcessor& transform();
};

}
