#pragma once

#include "Entity.h"

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

    void update(float systemDelta, float gameDelta);
    void draw(RenderQueue *ops);
    void cleanStep();

public:
    World();
    World(const std::string &worldResource);
    ~World();

    Entity spawn();
    Entity spawn(const std::string &entityResource);
    //Entity spawn(const std::string &entityResource /* position */);
    //Entity spawn(const std::string &entityResource /* transformation */);
    bool alive(Entity e);
    void destroy(Entity e);

    void pauseSimulation(bool paused);

    AudioComponentProcessor& audio();
    StaticMeshComponentProcessor& staticMesh();
    ParticleSystemComponentProcessor& vfx();
    PhysicsComponentProcessor& physics();
    TransformComponentProcessor& transform();
};

}
