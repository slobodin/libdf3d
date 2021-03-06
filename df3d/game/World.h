#pragma once

#include "Entity.h"
#include "WorldRenderingParams.h"
#include <df3d/lib/Utils.h>

namespace df3d {

namespace game_impl { class EntityLoader; }

class StaticMeshComponentProcessor;
class ParticleSystemComponentProcessor;
class PhysicsComponentProcessor;
class SceneGraphComponentProcessor;
class Sprite2DComponentProcessor;
class TagComponentProcessor;
struct RenderQueue;
class EntityComponentProcessor;
class Camera;
class TimeManager;
class EntityComponentLoader;

class World : NonCopyable
{
    friend class EngineController;
    friend class RenderManager;

    HandleBag m_entitiesMgr;
    unique_ptr<game_impl::EntityLoader> m_entityLoader;

    using ComponentProcessor = unique_ptr<EntityComponentProcessor>;

    ComponentProcessor m_staticMeshes;
    ComponentProcessor m_vfx;
    ComponentProcessor m_physics;
    ComponentProcessor m_sceneGraph;
    ComponentProcessor m_sprite2D;
    ComponentProcessor m_tags;

    shared_ptr<Camera> m_camera;
    WorldRenderingParams m_renderingParams;

    bool m_paused = false;

    unique_ptr<TimeManager> m_timeMgr;

    std::vector<ComponentProcessor> m_userProcessors;
    std::unordered_map<uintptr_t, EntityComponentProcessor *> m_userProcessorsLookup;

    PodArray<EntityComponentProcessor*> m_engineProcessors;

    void update();
    void collectRenderOperations(RenderQueue *ops);
    void cleanStep();

    World();
    void destroyWorld();

public:
    ~World();

    Entity spawn();
    Entity spawnFromFile(const char *entityResource);
    Entity spawnFromJson(const Json::Value &entityResource);
    bool alive(Entity e);
    void destroy(Entity e);
    void destroyWithChildren(Entity e);
    size_t getEntitiesCount();

    void pauseSimulation(bool paused);

    template<typename T>
    void addUserComponentProcessor(unique_ptr<T> processor)
    {
        auto idx = utils::getTypeId<T>();
        DF3D_ASSERT_MESS(!utils::contains_key(m_userProcessorsLookup, idx), "already have this component processor");

        m_userProcessors.push_back(std::move(processor));
        m_userProcessorsLookup.insert(std::make_pair(idx, m_userProcessors.back().get()));
    }

    template<typename T>
    T& getProcessor()
    {
        auto found = m_userProcessorsLookup.find(utils::getTypeId<T>());
        DF3D_ASSERT_MESS(found != m_userProcessorsLookup.end(), "failed to lookup a component data processor");
        return static_cast<T&>(*found->second);
    }

    template<typename T>
    T* getProcessorPtr()
    {
        auto found = m_userProcessorsLookup.find(utils::getTypeId<T>());
        if (found == m_userProcessorsLookup.end())
            return nullptr;
        return static_cast<T*>(found->second);
    }

    void registerEntityComponentLoader(Id name, unique_ptr<EntityComponentLoader> loader);

    void setCamera(shared_ptr<Camera> camera) { m_camera = camera; }
    void setRenderingParams(const WorldRenderingParams &params) { m_renderingParams = params; }

    shared_ptr<Camera> getCamera() { return m_camera; }
    WorldRenderingParams& getRenderingParams() { return m_renderingParams; }
    const WorldRenderingParams& getRenderingParams() const { return m_renderingParams; }

    StaticMeshComponentProcessor& staticMesh();
    ParticleSystemComponentProcessor& vfx();
    PhysicsComponentProcessor& physics();
    SceneGraphComponentProcessor& sceneGraph();
    Sprite2DComponentProcessor& sprite2d();
    TagComponentProcessor& tags();

    TimeManager& timeManager() { return *m_timeMgr; }
};

}
