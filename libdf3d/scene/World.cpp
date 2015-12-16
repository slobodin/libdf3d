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
    return m_entityManager->create();
}

Entity World::spawn(const std::string &entityResource)
{
    return m_entityManager->create();
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
