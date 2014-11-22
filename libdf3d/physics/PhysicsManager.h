#pragma once

FWD_MODULE_CLASS(base, Controller)
FWD_MODULE_CLASS(render, RenderManager)
FWD_MODULE_CLASS(scene, Node)

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

namespace df3d { namespace physics {

class DF3D_DLL PhysicsManager
{
    friend class base::Controller;
    friend class render::RenderManager;

    struct Impl;
    unique_ptr<Impl> m_pimpl;

    bool m_paused = false;

    PhysicsManager();
    ~PhysicsManager();

public:
    btDynamicsWorld *getWorld();
    void pauseSimulation(bool pause);

private:
    // These are only for controller.
    bool init();
    void shutdown();

    void update(float dt);
    void drawDebug();
};

class NodeMotionState : public btMotionState
{
    scene::Node *m_node;
    btTransform *m_transform;

public:
    NodeMotionState(scene::Node *node);
    ~NodeMotionState();

    void getWorldTransform(btTransform &worldTrans) const;
    void setWorldTransform(const btTransform &worldTrans);
};

inline btVector3 glmTobt(const glm::vec3 &v)
{
    return btVector3(v.x, v.y, v.z);
}

inline glm::vec3 btToGlm(const btVector3 &v)
{
    return glm::vec3(v.x(), v.y(), v.z());
}

} }