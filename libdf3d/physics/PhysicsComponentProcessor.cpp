#include "df3d_pch.h"
#include "PhysicsComponentProcessor.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <math/AABB.h>
#include <math/BoundingSphere.h>
#include <math/ConvexHull.h>
#include "impl/BulletInterface.h"
#include <base/EngineController.h>
#include <base/DebugConsole.h>
#include <game/ComponentDataHolder.h>

namespace df3d {

class PhysicsComponentMotionState : public btMotionState
{
    ComponentInstance m_transformComponent;
    btTransform m_transform;

public:
    PhysicsComponentMotionState()
    {
        // TODO_ecs: 
        assert(false);
        //auto orientation = node->transform()->getOrientation();
        //auto pos = node->transform()->getPosition();

        //m_transform = btTransform(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w), glmTobt(pos));
    }

    ~PhysicsComponentMotionState()
    {

    }

    void getWorldTransform(btTransform &worldTrans) const
    {
        worldTrans = m_transform;
    }

    void setWorldTransform(const btTransform &worldTrans)
    {
        const auto &v = worldTrans.getOrigin();
        const auto &rot = worldTrans.getRotation();

        //m_node->transform()->setOrientation(glm::quat(rot.w(), rot.x(), rot.y(), rot.z()));
        //m_node->transform()->setPosition(v.getX(), v.getY(), v.getZ());

        m_transform = worldTrans;
    }
};

struct PhysicsComponentProcessor::Impl
{
    btDefaultCollisionConfiguration *collisionConfiguration = nullptr;
    btCollisionDispatcher *dispatcher = nullptr;
    btBroadphaseInterface *overlappingPairCache = nullptr;
    btSequentialImpulseConstraintSolver *solver = nullptr;
    btDiscreteDynamicsWorld *dynamicsWorld = nullptr;

    physics_impl::BulletDebugDraw *debugDraw;

    struct Data
    {
        btRigidBody *body = nullptr;
    };

    ComponentDataHolder<Data> data;

    Impl()
    {
        collisionConfiguration = new btDefaultCollisionConfiguration();
        dispatcher = new btCollisionDispatcher(collisionConfiguration);
        overlappingPairCache = new btDbvtBroadphase();
        solver = new btSequentialImpulseConstraintSolver();
        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

        dynamicsWorld->setGravity(btVector3(0, 0, 0));

        debugDraw = new physics_impl::BulletDebugDraw();
        dynamicsWorld->setDebugDrawer(debugDraw);
    }

    ~Impl()
    {
        data.clear();

        for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
        {
            btCollisionObject *obj = dynamicsWorld->getCollisionObjectArray()[i];
            btRigidBody *body = btRigidBody::upcast(obj);
            if (body && body->getMotionState())
                delete body->getMotionState();
            if (body && body->getCollisionShape())
                delete body->getCollisionShape();

            dynamicsWorld->removeCollisionObject(obj);
            delete obj;
        }

        SAFE_DELETE(dynamicsWorld);
        SAFE_DELETE(solver);
        SAFE_DELETE(overlappingPairCache);
        SAFE_DELETE(dispatcher);
        SAFE_DELETE(collisionConfiguration);
        SAFE_DELETE(debugDraw);
    }
};

void PhysicsComponentProcessor::update(float systemDelta, float gameDelta)
{
    m_pimpl->dynamicsWorld->stepSimulation(gameDelta, 10);
}

void PhysicsComponentProcessor::cleanStep(World &w)
{

}

void PhysicsComponentProcessor::draw(RenderQueue *ops)
{
    if (!svc().debugConsole()->getCVars().get<bool>(CVAR_DEBUG_DRAW))
        return;

    // Collect render operations.
    m_pimpl->dynamicsWorld->debugDrawWorld();
    // Append to render queue.
    m_pimpl->debugDraw->flushRenderOperations(ops);
}

PhysicsComponentProcessor::PhysicsComponentProcessor()
    : m_pimpl(new Impl())
{

}

PhysicsComponentProcessor::~PhysicsComponentProcessor()
{
    // TODO_ecs: first, remove all physics actors!
    assert(false);
}

btRigidBody* PhysicsComponentProcessor::body(ComponentInstance comp)
{
    return nullptr;
}

void PhysicsComponentProcessor::add(Entity e, const CreationParams &params, const AABB &box)
{
    assert(false);
}

void PhysicsComponentProcessor::add(Entity e, const CreationParams &params, const BoundingSphere &sphere)
{
    assert(false);
}

void PhysicsComponentProcessor::add(Entity e, const CreationParams &params, const ConvexHull &hull)
{
    assert(false);
}

void PhysicsComponentProcessor::remove(Entity e)
{

}

btDynamicsWorld* PhysicsComponentProcessor::getPhysicsWorld()
{
    return m_pimpl->dynamicsWorld;
}

}
