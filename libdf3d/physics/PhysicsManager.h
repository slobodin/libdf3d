#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

namespace df3d {

class DF3D_DLL PhysicsManager : utils::NonCopyable
{
    struct Impl;
    unique_ptr<Impl> m_pimpl;

    bool m_paused = false;

public:
    PhysicsManager();
    ~PhysicsManager();

    btDynamicsWorld* getWorld();
    void pauseSimulation(bool pause);

    void update(float systemDelta, float gameDelta);
    void drawDebug();
};

class NodeMotionState : public btMotionState
{
    Node *m_node;
    btTransform m_transform;

public:
    NodeMotionState(Node *node);
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

}