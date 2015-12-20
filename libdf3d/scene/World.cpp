#include "df3d_pch.h"
#include "World.h"

#include "impl/EntityManager.h"
#include "impl/WorldLoader.h"
#include "impl/EntityLoader.h"
#include <audio/AudioComponentProcessor.h>
#include <render/StaticMeshComponentProcessor.h>
#include <particlesys/ParticleSystemComponentProcessor.h>
#include <physics/PhysicsComponentProcessor.h>
#include <scene/TransformComponentProcessor.h>
#include <scene/Camera.h>
#include <render/RenderQueue.h>

namespace df3d {

void World::update(float systemDelta, float gameDelta)
{
    if (m_paused)
        return;

    // TODO_ecs: ordering.
    m_physics->update(systemDelta, gameDelta);
    m_tranform->update(systemDelta, gameDelta);
    m_vfx->update(systemDelta, gameDelta);
    m_staticMeshes->update(systemDelta, gameDelta);
    m_audio->update(systemDelta, gameDelta);

    cleanStep();
}

void World::collectRenderOperations(RenderQueue *ops)
{
    // TODO: refactor light system.
    for (const auto& light : m_renderingParams.getLights())
        ops->lights.push_back(&light);

    // TODO_ecs: can do in parallel.
    staticMesh().draw(ops);
    vfx().draw(ops);
    physics().draw(ops);
}

void World::cleanStep()
{
    // TODO_ecs:
}

World::World()
    : m_entityManager(new scene_impl::EntityManager()),
    m_audio(new AudioComponentProcessor()),
    m_staticMeshes(new StaticMeshComponentProcessor()),
    m_vfx(new ParticleSystemComponentProcessor()),
    m_physics(new PhysicsComponentProcessor()),
    m_tranform(new TransformComponentProcessor()),
    m_camera(new Camera())
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
    return scene_impl::EntityLoader::createEntity(entityResource, *this);
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
    m_paused = paused;
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

unique_ptr<World> World::newWorld()
{
    return unique_ptr<World>(new World());
}

unique_ptr<World> World::newWorld(const std::string &worldResource)
{
    return scene_impl::WorldLoader::createWorld(worldResource);
}

}
