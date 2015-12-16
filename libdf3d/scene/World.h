#pragma once

#include "Entity.h"

namespace df3d {

class EntityManager;
class AudioComponentProcessor;
class StaticMeshComponentProcessor;
class ParticleSystemComponentProcessor;
class PhysicsComponentProcessor;
class TransformComponentProcessor;

class DF3D_DLL World : utils::NonCopyable
{
    unique_ptr<EntityManager> m_entityManager;

    unique_ptr<AudioComponentProcessor> m_audio;
    unique_ptr<StaticMeshComponentProcessor> m_staticMeshes;
    unique_ptr<ParticleSystemComponentProcessor> m_vfx;
    unique_ptr<PhysicsComponentProcessor> m_physics;
    unique_ptr<TransformComponentProcessor> m_tranform;

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

    AudioComponentProcessor& audio() { return *m_audio; }
    StaticMeshComponentProcessor& staticMesh() { return *m_staticMeshes; }
    ParticleSystemComponentProcessor& vfx() { return *m_vfx; }
    PhysicsComponentProcessor& physics() { return *m_physics; }
    TransformComponentProcessor& transform() { return *m_tranform; }
};

}
