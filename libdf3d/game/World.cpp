#include "World.h"

#include "impl/EntityManager.h"
#include "impl/EntityLoader.h"
#include <libdf3d/audio/AudioComponentProcessor.h>
#include <libdf3d/base/TimeManager.h>
#include <libdf3d/base/EngineController.h>
#include <libdf3d/2d/Sprite2DComponentProcessor.h>
#include <libdf3d/3d/StaticMeshComponentProcessor.h>
#include <libdf3d/3d/SceneGraphComponentProcessor.h>
#include <libdf3d/3d/Camera.h>
#include <libdf3d/particlesys/ParticleSystemComponentProcessor.h>
#include <libdf3d/game/TagComponentProcessor.h>
#include <libdf3d/physics/PhysicsComponentProcessor.h>
#include <libdf3d/render/RenderQueue.h>

namespace df3d {

void World::update()
{
    if (!m_paused)
    {
        m_physics->update();

        // Update client code.
        m_timeMgr->update(svc().timer().getFrameDelta(TimeChannel::GAME));
        for (auto &userProcessor : m_userProcessors)
            userProcessor.second->update();

        m_sceneGraph->update();
        m_vfx->update();
        m_staticMeshes->update();
        m_sprite2D->update();
        m_audio->update();
        m_tags->update();
    }

    cleanStep();
}

void World::collectRenderOperations(RenderQueue *ops)
{
    // TODO: refactor light system.
    for (const auto& light : m_renderingParams.getLights())
        ops->lights.push_back(&light);

    // TODO: can do in parallel.
    for (auto engineProcessor : m_engineProcessors)
        engineProcessor->draw(ops);
    for (auto &userProcessor : m_userProcessors)
        userProcessor.second->draw(ops);
}

void World::cleanStep()
{
    for (auto &userProcessor : m_userProcessors)
        userProcessor.second->cleanStep(m_recentlyRemovedEntities);

    for (auto engineProcessor : m_engineProcessors)
        engineProcessor->cleanStep(m_recentlyRemovedEntities);

    m_timeMgr->cleanStep();

    m_recentlyRemovedEntities.clear();
    m_entityManager->cleanStep();
}

World::World()
    : m_entityManager(new game_impl::EntityManager()),
    m_entityLoader(new game_impl::EntityLoader()),
    m_audio(new AudioComponentProcessor(this)),
    m_staticMeshes(new StaticMeshComponentProcessor(this)),
    m_vfx(new ParticleSystemComponentProcessor(this)),
    m_physics(new PhysicsComponentProcessor(this)),
    m_sceneGraph(new SceneGraphComponentProcessor()),
    m_sprite2D(new Sprite2DComponentProcessor(this)),
    m_tags(new TagComponentProcessor()),
    m_camera(new Camera()),
    m_timeMgr(new TimeManager())
{
    m_engineProcessors.push_back(m_audio.get());
    m_engineProcessors.push_back(m_staticMeshes.get());
    m_engineProcessors.push_back(m_vfx.get());
    m_engineProcessors.push_back(m_physics.get());
    m_engineProcessors.push_back(m_sceneGraph.get());
    m_engineProcessors.push_back(m_sprite2D.get());
    m_engineProcessors.push_back(m_tags.get());
}

void World::destroyWorld()
{
    m_userProcessors.clear();
    m_engineProcessors.clear();

    m_tags.reset();
    m_audio.reset();
    m_staticMeshes.reset();
    m_sprite2D.reset();
    m_vfx.reset();
    m_physics.reset();
    m_sceneGraph.reset();

    m_camera.reset();
    m_timeMgr.reset();

    DFLOG_DEBUG("World::destroyWorld alive entities: %d", m_entityManager->size());

    m_entityManager.reset();
}

World::~World()
{

}

Entity World::spawn()
{
    auto entity = m_entityManager->create();
    // NOTE: forcing to have transform component, otherwise have some problems (especially performance)
    // with systems that require transform component.
    ((SceneGraphComponentProcessor*)m_sceneGraph.get())->add(entity);

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
        DFLOG_WARN("Failed to destroy an entity. Entity is not alive!");
    }
}

void World::destroyWithChildren(Entity e)
{
    if (e.valid() && alive(e))
    {
        const auto &children = sceneGraph().getChildren(e);
        for (auto &child : children)
            destroyWithChildren(child);

        destroy(e);
    }
    else
    {
        DFLOG_WARN("Failed to destroy an entity. Entity is not alive!");
    }
}

size_t World::getEntitiesCount()
{
    return m_entityManager->size();
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

SceneGraphComponentProcessor& World::sceneGraph()
{
    return static_cast<SceneGraphComponentProcessor&>(*m_sceneGraph);
}

Sprite2DComponentProcessor& World::sprite2d()
{
    return static_cast<Sprite2DComponentProcessor&>(*m_sprite2D);
}

TagComponentProcessor& World::tags()
{
    return static_cast<TagComponentProcessor&>(*m_tags);
}

}
