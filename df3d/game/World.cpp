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
#include <df3d/engine/physics/PhysicsComponentProcessor.h>
#include <df3d/engine/render/RenderQueue.h>

namespace df3d {

struct World::EntitiesManager
{
    PodArray<uint32_t> generations;
    PodArray<uint32_t> freeList;
    PodArray<Entity> recentlyRemoved;
    size_t entitiesCount = 0;

#ifdef _DEBUG
    std::unordered_set<uint32_t> m_check;
#endif

    EntitiesManager()
        : generations(MemoryManager::allocDefault()),
        freeList(MemoryManager::allocDefault()),
        recentlyRemoved(MemoryManager::allocDefault())
    {

    }

    Entity getNew()
    {
        uint32_t idx;
        if (freeList.empty())
        {
            generations.push_back(1);
            idx = generations.size() - 1;
        }
        else
        {
            idx = freeList.back();
            freeList.pop_back();
        }

        ++entitiesCount;

        Entity retVal = { Handle(idx, generations[idx]) };
#ifdef _DEBUG
        DF3D_ASSERT(!df3d::utils::contains_key(m_check, retVal.getID()));
        m_check.insert(retVal.getID());
#endif
        return retVal;
    }

    void destroy(Entity e)
    {
#ifdef _DEBUG
        DF3D_ASSERT(utils::contains_key(m_check, e.getID()));
        m_check.erase(e.getID());
#endif

        DF3D_ASSERT(entitiesCount > 0);

        ++generations[e.handle.getIdx()];
        if (generations[e.handle.getIdx()] >= (1 << HANDLE_GENERATION_BITS))
            generations[e.handle.getIdx()] = 1;

        recentlyRemoved.push_back(e);

        --entitiesCount;
    }

    bool isAlive(Entity e) const
    {
        return (e.handle.getIdx() < generations.size()) && (generations[e.handle.getIdx()] == e.handle.getGeneration());
    }
};

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
    m_timeMgr->cleanStep();

    for (auto e : m_entitiesMgr->recentlyRemoved)
    {
        for (auto &engineProc : m_engineProcessors)
        {
            if (engineProc->has(e))
                engineProc->remove(e);
        }

        for (auto &userProc : m_userProcessors)
        {
            if (userProc.second->has(e))
                userProc.second->remove(e);
        }

        m_entitiesMgr->freeList.push_back(e.handle.getIdx());
    }

    m_entitiesMgr->recentlyRemoved.clear();
}

World::World()
    : m_entitiesMgr(new World::EntitiesManager()),
    m_entityLoader(new game_impl::EntityLoader()),
    m_staticMeshes(new StaticMeshComponentProcessor(this)),
    m_vfx(new ParticleSystemComponentProcessor(this)),
    m_physics(new PhysicsComponentProcessor(this)),
    m_sceneGraph(new SceneGraphComponentProcessor()),
    m_sprite2D(new Sprite2DComponentProcessor(this)),
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
    auto entity = m_entitiesMgr->getNew();
    // NOTE: forcing to have transform component, otherwise have some problems (especially performance)
    // with systems that require transform component.
    sceneGraph().add(entity);

    return entity;
}

Entity World::spawnFromFile(const char *entityResource)
{
    return m_entityLoader->createEntity(entityResource, *this);
}

Entity World::spawnFromJson(const Json::Value &entityResource)
{
    return m_entityLoader->createEntity(entityResource, *this);
}

bool World::alive(Entity e)
{
    return m_entitiesMgr->isAlive(e);
}

void World::destroy(Entity e)
{
    if (alive(e))
    {
        m_entitiesMgr->destroy(e);
    }
    else
    {
        DFLOG_WARN("Failed to destroy an entity. Entity is not alive!");
    }
}

void World::destroyWithChildren(Entity e)
{
    const auto &children = sceneGraph().getChildren(e);
    for (auto &child : children)
        destroyWithChildren(child);

    destroy(e);
}

size_t World::getEntitiesCount()
{
    return m_entitiesMgr->entitiesCount;
}

void World::pauseSimulation(bool paused)
{
    m_paused = paused;
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
