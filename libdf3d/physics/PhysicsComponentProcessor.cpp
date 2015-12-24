#include "PhysicsComponentProcessor.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "impl/BulletInterface.h"
#include "PhysicsComponentCreationParams.h"
#include "PhysicsHelpers.h"
#include <math/AABB.h>
#include <math/BoundingSphere.h>
#include <math/ConvexHull.h>
#include <render/MeshData.h>
#include <3d/StaticMeshComponentProcessor.h>
#include <3d/TransformComponentProcessor.h>
#include <base/EngineController.h>
#include <base/DebugConsole.h>
#include <base/TimeManager.h>
#include <game/ComponentDataHolder.h>
#include <game/World.h>

namespace df3d {

class PhysicsComponentMotionState : public btMotionState
{
    World &m_world;
    Entity m_holder;
    btTransform m_transform;

public:
    PhysicsComponentMotionState(Entity e, World &w)
        : m_world(w),
        m_holder(e)
    {
        auto orientation = w.transform().getOrientation(m_holder);
        auto position = w.transform().getPosition(m_holder);

        m_transform = btTransform(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w), glmTobt(position));
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

        // TODO_ecs: more fast lookup!
        m_world.transform().setOrientation(m_holder, glm::quat(rot.w(), rot.x(), rot.y(), rot.z()));
        m_world.transform().setPosition(m_holder, v.getX(), v.getY(), v.getZ());

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
        Entity holder;

        btRigidBody *body = nullptr;
        bool initialized = false;
        weak_ptr<MeshData> meshData;
        PhysicsComponentCreationParams params;  // TODO_ecs: can delete it in order to minimize size.
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

        delete dynamicsWorld;
        delete solver;
        delete overlappingPairCache;
        delete dispatcher;
        delete collisionConfiguration;
        delete debugDraw;
    }

    void initialize(Data &data)
    {
        assert(!data.initialized);

        btCollisionShape *colShape = nullptr;
        switch (data.params.type)
        {
        case CollisionShapeType::BOX:
        {
            auto aabb = svc().world().staticMesh().getAABB(data.holder);
            if (!aabb.isValid())
            {
                glog << "Can not create box shape for rigid body. AABB is invalid" << logwarn;
                return;
            }

            auto half = (aabb.maxPoint() - aabb.minPoint()) / 2.0f;
            colShape = new btBoxShape(btVector3(half.x, half.y, half.z));
        }
        break;
        case CollisionShapeType::SPHERE:
        {
            auto sphere = svc().world().staticMesh().getBoundingSphere(data.holder);
            if (!sphere.isValid())
            {
                glog << "Can not create sphere shape for rigid body. Bounding sphere is invalid" << logwarn;
                return;
            }

            auto radius = sphere.getRadius();
            colShape = new btSphereShape(radius);
        }
        break;
        case CollisionShapeType::CONVEX_HULL:
        {
            auto convexHull = data.meshData.lock()->getConvexHull();
            if (!convexHull || !convexHull->isValid())
            {
                glog << "Can not create convex hull shape for rigid body. Hull is invalid" << logwarn;
                return;
            }

            const auto &vertices = convexHull->getVertices();
            std::vector<btVector3> tempPoints;
            for (const auto &v : vertices)
                tempPoints.push_back({ v.x, v.y, v.z });

            colShape = new btConvexHullShape((btScalar*)tempPoints.data(), tempPoints.size());

            // FIXME: what to do if scale has changed?
            auto scale = svc().world().transform().getScale(data.holder);
            colShape->setLocalScaling(glmTobt(scale));
        }
        break;
        default:
            assert(false);
            return;
        }

        btVector3 localInertia(0, 0, 0);
        if (!glm::epsilonEqual(data.params.mass, 0.0f, glm::epsilon<float>()))
            colShape->calculateLocalInertia(data.params.mass, localInertia);

        // Set motion state.
        auto myMotionState = new PhysicsComponentMotionState(data.holder, world());

        // Fill body properties.
        btRigidBody::btRigidBodyConstructionInfo rbInfo(data.params.mass, myMotionState, colShape, localInertia);
        rbInfo.m_friction = data.params.friction;
        rbInfo.m_restitution = data.params.restitution;
        rbInfo.m_linearDamping = data.params.linearDamping;
        rbInfo.m_angularDamping = data.params.angularDamping;

        data.body = new btRigidBody(rbInfo);

        if (data.params.group != -1 && data.params.mask != -1)
            dynamicsWorld->addRigidBody(data.body, data.params.group, data.params.mask);
        else
            dynamicsWorld->addRigidBody(data.body);

        if (data.params.disableDeactivation)
            data.body->setActivationState(DISABLE_DEACTIVATION);

        data.initialized = true;
    }
};

void PhysicsComponentProcessor::update()
{
    for (auto &data : m_pimpl->data.rawData())
    {
        if (!data.initialized)
        {
            if (data.meshData.lock()->isInitialized())
                m_pimpl->initialize(data);
        }
    }

    m_pimpl->dynamicsWorld->stepSimulation(svc().timer().getFrameDelta(TimeChannel::GAME), 10);
}

void PhysicsComponentProcessor::cleanStep(const std::list<Entity> &deleted)
{
    m_pimpl->data.cleanStep(deleted);
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
    m_pimpl->data.setDestructionCallback([this](const Impl::Data &data) {
        if (data.body)
        {
            getPhysicsWorld()->removeRigidBody(data.body);
            auto motionState = data.body->getMotionState();
            delete motionState;
            auto shape = data.body->getCollisionShape();
            delete shape;
            delete data.body;
        }
    });
}

PhysicsComponentProcessor::~PhysicsComponentProcessor()
{
    glog << "PhysicsComponentProcessor::~PhysicsComponentProcessor alive entities" << m_pimpl->data.rawData().size() << logdebug;
}

btRigidBody* PhysicsComponentProcessor::getBody(Entity e)
{
    return m_pimpl->data.getData(e).body;
}

void PhysicsComponentProcessor::teleport(Entity e, const glm::vec3 &pos)
{
    auto body = getBody(e);
    auto tr = body->getWorldTransform();
    tr.setOrigin(glmTobt(pos));

    body->setWorldTransform(tr);
}

void PhysicsComponentProcessor::add(Entity e, const PhysicsComponentCreationParams &params, shared_ptr<MeshData> meshData)
{
    if (m_pimpl->data.contains(e))
    {
        glog << "An entity already has an physics component" << logwarn;
        return;
    }

    Impl::Data data;
    data.meshData = meshData;
    data.holder = e;
    data.params = params;

    if (meshData->isInitialized())
        m_pimpl->initialize(data);

    m_pimpl->data.add(e, data);
}

void PhysicsComponentProcessor::remove(Entity e)
{
    if (!m_pimpl->data.contains(e))
    {
        glog << "Failed to remove physics component from an entity. Component is not attached" << logwarn;
        return;
    }

    m_pimpl->data.remove(e);
}

btDynamicsWorld* PhysicsComponentProcessor::getPhysicsWorld()
{
    return m_pimpl->dynamicsWorld;
}

}
