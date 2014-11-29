#include "df3d_pch.h"
#include "PhysicsManager.h"

#include "BulletInterface.h"
#include <scene/Node.h>
#include <components/TransformComponent.h>

namespace df3d { namespace physics {

struct PhysicsManager::Impl
{
    btDefaultCollisionConfiguration *m_collisionConfiguration = nullptr;
    btCollisionDispatcher *m_dispatcher = nullptr;
    btBroadphaseInterface *m_overlappingPairCache = nullptr;
    btSequentialImpulseConstraintSolver *m_solver = nullptr;
    btDiscreteDynamicsWorld *m_dynamicsWorld = nullptr;

    BulletDebugDraw *m_debugDraw;

    ~Impl()
    {
        clean();
    }

    void init()
    {
        m_collisionConfiguration = new btDefaultCollisionConfiguration();
        m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
        m_overlappingPairCache = new btDbvtBroadphase();
        m_solver = new btSequentialImpulseConstraintSolver();
        m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);

        m_dynamicsWorld->setGravity(btVector3(0, 0, 0));

        m_debugDraw = new BulletDebugDraw();
        m_dynamicsWorld->setDebugDrawer(m_debugDraw);
    }

    void clean()
    {
        if (!m_dynamicsWorld)
            return;

        for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
        {
            btCollisionObject *obj = m_dynamicsWorld->getCollisionObjectArray()[i];
            btRigidBody *body = btRigidBody::upcast(obj);
            if (body && body->getMotionState())
                delete body->getMotionState();
            if (body && body->getCollisionShape())
                delete body->getCollisionShape();

            m_dynamicsWorld->removeCollisionObject(obj);
            delete obj;
        }

        SAFE_DELETE(m_dynamicsWorld);
        SAFE_DELETE(m_solver);
        SAFE_DELETE(m_overlappingPairCache);
        SAFE_DELETE(m_dispatcher);
        SAFE_DELETE(m_collisionConfiguration);
        SAFE_DELETE(m_debugDraw);
    }
};

PhysicsManager::PhysicsManager()
    : m_pimpl(new Impl())
{

}

PhysicsManager::~PhysicsManager()
{

}

btDynamicsWorld *PhysicsManager::getWorld()
{
    return m_pimpl->m_dynamicsWorld;
}

void PhysicsManager::pauseSimulation(bool pause)
{
    m_paused = pause;
}

bool PhysicsManager::init()
{
    base::glog << "Initializing bullet physics" << base::logmess;
    m_pimpl->init();

    return true;
}

void PhysicsManager::shutdown()
{
    m_pimpl->clean();
}

void PhysicsManager::update(float dt)
{
    if (!m_paused)
        m_pimpl->m_dynamicsWorld->stepSimulation(dt, 10);
}

void PhysicsManager::drawDebug()
{
    // Collect render operations.
    m_pimpl->m_dynamicsWorld->debugDrawWorld();
    // Flush.
    m_pimpl->m_debugDraw->flushRenderOperations();
}

NodeMotionState::NodeMotionState(scene::Node *node)
    : m_node(node)
{
    auto orientation = node->transform()->getOrientation();
    auto pos = node->transform()->getPosition();

    m_transform = new btTransform(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w), glmTobt(pos));
}

NodeMotionState::~NodeMotionState()
{
    delete m_transform;
}

void NodeMotionState::getWorldTransform(btTransform &worldTrans) const
{
    worldTrans = *m_transform;
}

void NodeMotionState::setWorldTransform(const btTransform &worldTrans)
{
    const auto &v = worldTrans.getOrigin();
    const auto &rot = worldTrans.getRotation();

    m_node->transform()->setOrientation(glm::quat(rot.w(), rot.x(), rot.y(), rot.z()));
    m_node->transform()->setPosition(v.getX(), v.getY(), v.getZ());

    *m_transform = worldTrans;
}

} }