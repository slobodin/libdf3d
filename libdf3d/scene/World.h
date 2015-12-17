#pragma once

#include "Entity.h"
#include "Camera.h"

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

    unique_ptr<EntityManager> m_entityManager;

    unique_ptr<EntityComponentProcessor> m_audio;
    unique_ptr<EntityComponentProcessor> m_staticMeshes;
    unique_ptr<EntityComponentProcessor> m_vfx;
    unique_ptr<EntityComponentProcessor> m_physics;
    unique_ptr<EntityComponentProcessor> m_tranform;

    Camera m_camera;

    void update(float systemDelta, float gameDelta);
    void draw(RenderQueue *ops);
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
    Camera& getCamera() { return m_camera; }
    const Camera& getCamera() const { return m_camera; }

    AudioComponentProcessor& audio();
    StaticMeshComponentProcessor& staticMesh();
    ParticleSystemComponentProcessor& vfx();
    PhysicsComponentProcessor& physics();
    TransformComponentProcessor& transform();
};

}
