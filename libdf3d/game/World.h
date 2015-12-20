#pragma once

#include "Entity.h"
#include "WorldRenderingParams.h"

namespace df3d {

namespace scene_impl { class EntityManager; }

class AudioComponentProcessor;
class StaticMeshComponentProcessor;
class ParticleSystemComponentProcessor;
class PhysicsComponentProcessor;
class TransformComponentProcessor;
class RenderQueue;
class EntityComponentProcessor;
class Camera;

class DF3D_DLL World : utils::NonCopyable
{
    friend class EngineController;
    friend class RenderManager;

    unique_ptr<scene_impl::EntityManager> m_entityManager;

    unique_ptr<EntityComponentProcessor> m_audio;
    unique_ptr<EntityComponentProcessor> m_staticMeshes;
    unique_ptr<EntityComponentProcessor> m_vfx;
    unique_ptr<EntityComponentProcessor> m_physics;
    unique_ptr<EntityComponentProcessor> m_tranform;

    shared_ptr<Camera> m_camera;
    WorldRenderingParams m_renderingParams;

    bool m_paused = false;

    void update(float systemDelta, float gameDelta);
    void collectRenderOperations(RenderQueue *ops);
    void cleanStep();

    World();

public:
    ~World();

    Entity spawn();
    Entity spawn(const std::string &entityResource);
    Entity spawn(const Json::Value &entityResource);
    bool alive(Entity e);
    void destroy(Entity e);

    void pauseSimulation(bool paused);

    void setCamera(shared_ptr<Camera> camera) { m_camera = camera; }
    void setRenderingParams(const WorldRenderingParams &params) { m_renderingParams = params; }

    Camera& getCamera() { return *m_camera; }
    const Camera& getCamera() const { return *m_camera; }

    WorldRenderingParams& getRenderingParams() { return m_renderingParams; }
    const WorldRenderingParams& getRenderingParams() const { return m_renderingParams; }

    AudioComponentProcessor& audio();
    StaticMeshComponentProcessor& staticMesh();
    ParticleSystemComponentProcessor& vfx();
    PhysicsComponentProcessor& physics();
    TransformComponentProcessor& transform();

    static unique_ptr<World> newWorld();
    static unique_ptr<World> newWorld(const std::string &worldResource);
};

}
