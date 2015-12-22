#include "df3d_pch.h"
#include "World.h"

#include "impl/EntityManager.h"
#include "impl/WorldLoader.h"
#include "impl/EntityLoader.h"
#include <audio/AudioComponentProcessor.h>
#include <base/TimeManager.h>
#include <3d/StaticMeshComponentProcessor.h>
#include <3d/TransformComponentProcessor.h>
#include <3d/Camera.h>
#include <particlesys/ParticleSystemComponentProcessor.h>
#include <physics/PhysicsComponentProcessor.h>
#include <render/RenderQueue.h>

namespace df3d {

void World::update()
{
    // TODO_ecs: ordering.
    // TODO_ecs: pause check!!!
    if (!m_paused)
    {
        m_timeMgr->update();    // Update client code.

        m_physics->update();
        m_tranform->update();
        m_vfx->update();
        m_staticMeshes->update();
        m_audio->update();
    }

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
    //m_physics->cleanStep(*this);
    //m_tranform->cleanStep(*this);
    //m_vfx->cleanStep(*this);
    //m_staticMeshes->cleanStep(*this);
    //m_audio->cleanStep(*this);

    m_timeMgr->cleanStep();
}

World::World()
    : m_entityManager(new scene_impl::EntityManager()),
    m_audio(new AudioComponentProcessor()),
    m_staticMeshes(new StaticMeshComponentProcessor()),
    m_vfx(new ParticleSystemComponentProcessor()),
    m_physics(new PhysicsComponentProcessor()),
    m_tranform(new TransformComponentProcessor()),
    m_camera(new Camera()),
    m_timeMgr(new TimeManager())
{

}

void World::destroy()
{
    // TODO_ecs: first, clean user systems.

    m_audio.reset();
    m_staticMeshes.reset();
    m_vfx.reset();
    m_physics.reset();
    m_tranform.reset();

    m_camera.reset();
    m_entityManager.reset();

    m_timeMgr.reset();
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

Entity World::spawn(const Json::Value &entityResource)
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
