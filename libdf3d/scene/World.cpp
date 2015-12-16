#include "df3d_pch.h"
#include "World.h"

#include "impl/EntityManager.h"
#include <audio/AudioComponentProcessor.h>
#include <render/StaticMeshComponentProcessor.h>
#include <particlesys/ParticleSystemComponentProcessor.h>
#include <physics/PhysicsComponentProcessor.h>
#include <scene/TransformComponentProcessor.h>

namespace df3d {

World::World()
    : m_entityManager(new EntityManager()),
    m_audio(new AudioComponentProcessor()),
    m_staticMeshes(new StaticMeshComponentProcessor()),
    m_vfx(new ParticleSystemComponentProcessor()),
    m_physics(new PhysicsComponentProcessor()),
    m_tranform(new TransformComponentProcessor())
{

}

World::World(const std::string &worldResource)
{

}

World::~World()
{

}

Entity World::spawn()
{
    auto entity = m_entityManager->create();
    // NOTE: forcing have transform component, otherwise have some problems (especially performance)
    // with systems that require transform component.
    m_tranform->add(entity);

    return entity;
}

Entity World::spawn(const std::string &entityResource)
{
    auto entity = spawn();

    return entity;
}

bool World::alive(Entity e)
{
    return m_entityManager->alive(e);
}

void World::destroy(Entity e)
{
    m_entityManager->destroy(e);
}

}
