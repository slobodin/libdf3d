#include "World.h"

#include "impl/EntityManager.h"
#include "impl/EntityLoader.h"
#include "DebugNameComponentProcessor.h"
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
        m_debugName->update();
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
    // TODO_ecs: clean for user processors.
    m_physics->cleanStep(m_recentlyRemovedEntities);
    m_tranform->cleanStep(m_recentlyRemovedEntities);
    m_vfx->cleanStep(m_recentlyRemovedEntities);
    m_staticMeshes->cleanStep(m_recentlyRemovedEntities);
    m_audio->cleanStep(m_recentlyRemovedEntities);
    m_debugName->cleanStep(m_recentlyRemovedEntities);

    m_timeMgr->cleanStep();

    m_recentlyRemovedEntities.clear();
}

World::World()
    : m_entityManager(new game_impl::EntityManager()),
    m_entityLoader(new game_impl::EntityLoader()),
    m_audio(new AudioComponentProcessor(this)),
    m_staticMeshes(new StaticMeshComponentProcessor(this)),
    m_vfx(new ParticleSystemComponentProcessor(this)),
    m_physics(new PhysicsComponentProcessor()),
    m_tranform(new TransformComponentProcessor()),
    m_debugName(new DebugNameComponentProcessor()),
    m_camera(new Camera()),
    m_timeMgr(new TimeManager())
{

}

void World::destroyWorld()
{
    // TODO_ecs: first, clean user systems.

    m_audio.reset();
    m_staticMeshes.reset();
    m_vfx.reset();
    m_physics.reset();
    m_tranform.reset();
    m_debugName.reset();

    m_camera.reset();
    m_timeMgr.reset();

    glog << "World::destroyWorld alive entities:" << m_entityManager->size() << logdebug;

    m_entityManager.reset();
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
    return m_entityLoader->createEntity(entityResource, *this);
}

Entity World::spawn(const Json::Value &entityResource)
{
    return m_entityLoader->createEntity(entityResource, *this);
}

bool World::alive(Entity e)
{
    return m_entityManager->alive(e);
}

void World::destroy(Entity e)
{
    if (e.valid() && alive(e))
    {
        m_entityManager->destroy(e);
        m_recentlyRemovedEntities.push_back(e);
    }
    else
    {
        glog << "Failed to destroy an entity. Entity is not alive!" << logwarn;
    }
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

DebugNameComponentProcessor& World::debugName()
{
    return static_cast<DebugNameComponentProcessor&>(*m_debugName);
}

}
