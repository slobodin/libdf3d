#include "df3d_pch.h"
#include "World.h"

#include "impl/EntityManager.h"
#include <audio/AudioComponentProcessor.h>
#include <render/StaticMeshComponentProcessor.h>
#include <particlesys/ParticleSystemComponentProcessor.h>
#include <physics/PhysicsComponentProcessor.h>
#include <scene/TransformComponentProcessor.h>

namespace df3d {

void World::update(float systemDelta, float gameDelta)
{
    // TODO: Update client processors.
    // TODO_ecs: ordering.

    m_physics->update(systemDelta, gameDelta);
    m_tranform->update(systemDelta, gameDelta);
    m_vfx->update(systemDelta, gameDelta);
    m_staticMeshes->update(systemDelta, gameDelta);
    m_audio->update(systemDelta, gameDelta);
}

void World::draw(RenderQueue *ops)
{
    m_staticMeshes->draw(ops);
}

void World::cleanStep()
{

}

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
    ((TransformComponentProcessor*)m_tranform.get())->add(entity);

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

void World::pauseSimulation(bool paused)
{
    // TODO_ecs:
    assert(false);
}

AudioComponentProcessor& World::audio() 
{ 
    return static_cast<AudioComponentProcessor&>(*m_audio); 
}

StaticMeshComponentProcessor& World::staticMesh()
{
    return static_cast<StaticMeshComponentProcessor&>(*m_staticMeshes); 
}

ParticleSystemComponentProcessor& World::vfx()
{
    return static_cast<ParticleSystemComponentProcessor&>(*m_vfx); 
}

PhysicsComponentProcessor& World::physics()
{ 
    return static_cast<PhysicsComponentProcessor&>(*m_physics); 
}

TransformComponentProcessor& World::transform()
{ 
    return static_cast<TransformComponentProcessor&>(*m_tranform);
}

}
