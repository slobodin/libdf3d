#include "World.h"

#include "impl/EntityLoader.h"
#include <df3d/engine/TimeManager.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/2d/Sprite2DComponentProcessor.h>
#include <df3d/engine/3d/StaticMeshComponentProcessor.h>
#include <df3d/engine/3d/SceneGraphComponentProcessor.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/engine/particlesys/ParticleSystemComponentProcessor.h>
#include <df3d/game/TagComponentProcessor.h>
#include <df3d/game/EntityComponentLoader.h>
#include <df3d/engine/physics/PhysicsComponentProcessor.h>
#include <df3d/engine/render/RenderQueue.h>
#include <df3d/lib/Log.h>

namespace df3d {

void World::update()
{
    if (!m_paused)
    {
        m_physics->update();

        // Update client code.
        m_timeMgr->update(svc().timer().getFrameDelta(TIME_CHANNEL_GAME));
        for (auto &userProcessor : m_userProcessors)
            userProcessor.second->update();

        m_sceneGraph->update();
        m_vfx->update();
        m_staticMeshes->update();
        m_sprite2D->update();
        m_tags->update();
    }

    cleanStep();
}

void World::collectRenderOperations(RenderQueue *ops)
{
    // TODO: refactor light system.
    const auto &lights = m_renderingParams.getLights();
    for (size_t i = 0; i < LIGHTS_MAX; i++)
    {
        if (i >= lights.size())
            break;

        ops->lights[i] = lights[i];
    }

    // TODO: can do in parallel.
    for (auto engineProcessor : m_engineProcessors)
        engineProcessor->draw(ops);
    for (auto &userProcessor : m_userProcessors)
        userProcessor.second->draw(ops);
}

void World::cleanStep()
{
    m_timeMgr->cleanStep();
}

World::World()
    : m_entitiesMgr(df3d::MemoryManager::allocDefault()),
    m_entityLoader(new game_impl::EntityLoader()),
    m_staticMeshes(new StaticMeshComponentProcessor(*this)),
    m_vfx(new ParticleSystemComponentProcessor(*this)),
    m_physics(new PhysicsComponentProcessor(*this)),
    m_sceneGraph(new SceneGraphComponentProcessor(*this)),
    m_sprite2D(new Sprite2DComponentProcessor(*this)),
    m_tags(new TagComponentProcessor()),
    m_camera(new Camera(glm::vec3(), Camera::DEFAULT_FOV, Camera::DEFAULT_NEAR_Z, Camera::DEFAULT_FAR_Z)),
    m_timeMgr(new TimeManager()),
    m_engineProcessors(MemoryManager::allocDefault())
{
    m_engineProcessors.push_back(m_staticMeshes.get());
    m_engineProcessors.push_back(m_vfx.get());
    m_engineProcessors.push_back(m_physics.get());
    m_engineProcessors.push_back(m_sceneGraph.get());
    m_engineProcessors.push_back(m_sprite2D.get());
    m_engineProcessors.push_back(m_tags.get());
}

void World::destroyWorld()
{
    for (auto &kv : m_userProcessors)
        kv.second.reset();
    m_userProcessors.clear();
    m_engineProcessors.clear();

    m_tags.reset();
    m_staticMeshes.reset();
    m_sprite2D.reset();
    m_vfx.reset();
    m_physics.reset();
    m_sceneGraph.reset();

    m_camera.reset();
    m_timeMgr.reset();

    m_entitiesMgr.reset();
}

World::~World()
{

}

Entity World::spawn()
{
    auto entity = Entity(m_entitiesMgr.getNew());
    // NOTE: forcing to have transform component, otherwise have some problems (especially performance)
    // with systems that require transform component.
    sceneGraph().add(entity);

    return entity;
}

Entity World::spawnFromFile(const char *entityResource)
{
    return m_entityLoader->createEntityFromFile(entityResource, *this);
}

Entity World::spawnFromJson(const Json::Value &entityResource)
{
    return m_entityLoader->createEntityFromJson(entityResource, *this);
}

bool World::alive(Entity e)
{
    if (!e.isValid())
        return false;
    return m_entitiesMgr.isValid(e.getID());
}

void World::destroy(Entity e)
{
    if (alive(e))
    {
        for (auto &engineProc : m_engineProcessors)
        {
            if (engineProc->has(e))
                engineProc->remove(e);
        }
        for (auto &userProc : m_userProcessors)
        {
            if (userProc.second && userProc.second->has(e))
                userProc.second->remove(e);
        }

        m_entitiesMgr.release(e.getID());
    }
    else
    {
        DFLOG_WARN("Failed to destroy an entity. Entity is not alive!");
    }
}

void World::destroyWithChildren(Entity e)
{
    // Must copy children array.
    auto children = sceneGraph().getChildren(e);
    for (auto &child : children)
        destroyWithChildren(child);

    destroy(e);
}

size_t World::getEntitiesCount()
{
    return m_entitiesMgr.getSize();
}

void World::pauseSimulation(bool paused)
{
    m_paused = paused;
}

void World::registerEntityComponentLoader(Id name, unique_ptr<EntityComponentLoader> loader)
{
    m_entityLoader->registerEntityComponentLoader(name, std::move(loader));
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
