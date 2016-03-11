#pragma once

#include <typeindex>

#include "Entity.h"
#include "WorldRenderingParams.h"
#include <libdf3d/utils/Utils.h>

namespace df3d {

namespace game_impl { class EntityManager; class EntityLoader; }

class AudioComponentProcessor;
class StaticMeshComponentProcessor;
class ParticleSystemComponentProcessor;
class PhysicsComponentProcessor;
class SceneGraphComponentProcessor;
class Sprite2DComponentProcessor;
class TagComponentProcessor;
class RenderQueue;
class EntityComponentProcessor;
class Camera;
class TimeManager;

class DF3D_DLL World : utils::NonCopyable
{
    friend class EngineController;
    friend class RenderManager;

    unique_ptr<game_impl::EntityManager> m_entityManager;
    unique_ptr<game_impl::EntityLoader> m_entityLoader;

    using ComponentProcessor = unique_ptr<EntityComponentProcessor>;

    ComponentProcessor m_audio;
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

    std::list<Entity> m_recentlyRemovedEntities;

    std::unordered_map<std::type_index, ComponentProcessor> m_userProcessors;
    std::vector<EntityComponentProcessor*> m_engineProcessors;

    void update();
    void collectRenderOperations(RenderQueue *ops);
    void cleanStep();

    World();
    void destroyWorld();

public:
    ~World();

    Entity spawn();
    Entity spawn(const std::string &entityResource);
    Entity spawn(const Json::Value &entityResource);
    bool alive(Entity e);
    void destroy(Entity e);
    void destroyWithChildren(Entity e);
    size_t getEntitiesCount();

    void pauseSimulation(bool paused);

    void addUserComponentProcessor(unique_ptr<EntityComponentProcessor> processor);
    template<typename T>
    T& getProcessor()
    {
        // TODO_ecs: use utils::getTypeId, no rtti.
        auto found = m_userProcessors.find(std::type_index(typeid(T)));
        DF3D_ASSERT(found != m_userProcessors.end(), "failed to lookup a component data processor");
        return static_cast<T&>(*found->second);
    }

    void setCamera(shared_ptr<Camera> camera) { m_camera = camera; }
    void setRenderingParams(const WorldRenderingParams &params) { m_renderingParams = params; }

    shared_ptr<Camera> getCamera() { return m_camera; }

    WorldRenderingParams& getRenderingParams() { return m_renderingParams; }
    const WorldRenderingParams& getRenderingParams() const { return m_renderingParams; }

    AudioComponentProcessor& audio();
    StaticMeshComponentProcessor& staticMesh();
    ParticleSystemComponentProcessor& vfx();
    PhysicsComponentProcessor& physics();
    SceneGraphComponentProcessor& sceneGraph();
    Sprite2DComponentProcessor& sprite2d();
    TagComponentProcessor& tags();

    TimeManager& timeManager() { return *m_timeMgr; }
};

}
