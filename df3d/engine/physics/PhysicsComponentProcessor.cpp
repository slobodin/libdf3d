#include "PhysicsComponentProcessor.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <ConvexDecomposition/ConvexDecomposition.h>
#include "BulletInterface.h"
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include "PhysicsComponentCreationParams.h"
#include "PhysicsHelpers.h"
#include <df3d/lib/math/AABB.h>
#include <df3d/lib/math/BoundingSphere.h>
#include <df3d/lib/JsonUtils.h>
#include <df3d/engine/3d/SceneGraphComponentProcessor.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/EngineCVars.h>
#include <df3d/engine/TimeManager.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/MeshResource.h>
#include <df3d/game/ComponentDataHolder.h>
#include <df3d/game/World.h>

namespace df3d {

static btStridingMeshInterface* ShallowCopyBulletMeshData(btTriangleIndexVertexArray *input, Allocator &allocator)
{
    auto result = MAKE_NEW(allocator, btTriangleIndexVertexArray)();

    for (int i = 0; i < input->getNumSubParts(); i++)
    {
        const auto &part = input->getIndexedMeshArray()[i];
        result->addIndexedMesh(part, part.m_indexType);
    }

    return result;
}

ATTRIBUTE_ALIGNED16(class) PhysicsComponentMotionState : public btMotionState
{
    btTransform m_transform;
    World &m_world;
    Entity m_holder;

public:
    BT_DECLARE_ALIGNED_ALLOCATOR();

    PhysicsComponentMotionState(Entity e, World &w)
        : m_world(w),
        m_holder(e)
    {
        auto orientation = w.sceneGraph().getWorldOrientation(m_holder);
        auto position = w.sceneGraph().getWorldPosition(m_holder);

        m_transform = btTransform(PhysicsHelpers::glmTobt(orientation), PhysicsHelpers::glmTobt(position));
    }

    ~PhysicsComponentMotionState()
    {

    }

    void getWorldTransform(btTransform &worldTrans) const override
    {
        auto orientation = m_world.sceneGraph().getWorldOrientation(m_holder);
        auto position = m_world.sceneGraph().getWorldPosition(m_holder);

        worldTrans.setOrigin(PhysicsHelpers::glmTobt(position));
        worldTrans.setRotation(PhysicsHelpers::glmTobt(orientation));
    }

    void setWorldTransform(const btTransform &worldTrans) override
    {
        m_world.sceneGraph().setWorldTransform(m_holder, worldTrans);
    }
};

ATTRIBUTE_ALIGNED16(class) PhysicsComponentMotionStateKinematic : public btMotionState
{
    World &m_world;
    Entity m_holder;

public:
    BT_DECLARE_ALIGNED_ALLOCATOR();

    PhysicsComponentMotionStateKinematic(Entity e, World &w)
        : m_world(w),
        m_holder(e)
    {
    }

    ~PhysicsComponentMotionStateKinematic()
    {
    }

    void getWorldTransform(btTransform &worldTrans) const
    {
        auto orientation = m_world.sceneGraph().getWorldOrientation(m_holder);
        auto position = m_world.sceneGraph().getWorldPosition(m_holder);
        worldTrans = btTransform(PhysicsHelpers::glmTobt(orientation), PhysicsHelpers::glmTobt(position));
    }

    void setWorldTransform(const btTransform &worldTrans)
    {
        DF3D_ASSERT(false);
    }
};

PhysicsConfig::PhysicsConfig(const std::string &physicsConfigPath)
{
    auto jsonVal = JsonUtils::fromFile(physicsConfigPath.c_str());
    if (!jsonVal.isNull())
    {
        m_gravity = JsonUtils::get(jsonVal, "gravity", m_gravity);

        const auto &jsonCollisionGroups = jsonVal["collisionGroups"];
        short currGroup = 1;
        for (auto it = jsonCollisionGroups.begin(); it != jsonCollisionGroups.end(); ++it)
        {
            auto groupid = Id(it.key().asCString());
            DF3D_ASSERT(!utils::contains_key(m_collisionGroups, groupid));

            m_collisionGroups[groupid].first = currGroup;
            currGroup <<= 1;
        }

        DF3D_ASSERT(m_collisionGroups.size() < 16);

        for (auto it = jsonCollisionGroups.begin(); it != jsonCollisionGroups.end(); ++it)
        {
            auto groupid = Id(it.key().asCString());
            short mask = 0;
            for (const auto &collidesWith : *it)
            {
                auto found = m_collisionGroups.find(Id(collidesWith.asCString()));
                if (found != m_collisionGroups.end())
                {
                    mask |= found->second.first;
                }
                else
                    DF3D_ASSERT(false);
            }

            m_collisionGroups[groupid].second = mask;
        }
    }
    else
        DF3D_ASSERT_MESS(false, "Failed to find physics config");
}

const std::pair<short, short>* PhysicsConfig::getGroupMask(Id groupId) const
{
    auto found = m_collisionGroups.find(groupId);
    if (found != m_collisionGroups.end())
        return &found->second;
    return nullptr;
}

void PhysicsComponentProcessor::addRigidBodyToWorld(btRigidBody *body, Id groupId)
{
    if (auto groupMask = m_config.getGroupMask(groupId))
    {
        addRigidBodyToWorld(body, groupMask->first, groupMask->second);
    }
    else
    {
        // No collision filtering.
        DF3D_ASSERT(false);
        addRigidBodyToWorld(body, 0, 0);
    }
}

void PhysicsComponentProcessor::addRigidBodyToWorld(btRigidBody *body, short group, short mask)
{
    m_dynamicsWorld->addRigidBody(body, group, mask);
}

btCollisionShape* PhysicsComponentProcessor::createCollisionShape(Data &data, df3d::Id meshResourceId, const PhysicsComponentCreationParams &params)
{
    // FIXME: what to do if scale has been changed?
    auto wTransf = m_df3dWorld.sceneGraph().getWorldTransform(data.holder);
    auto scale = PhysicsHelpers::glmTobt(wTransf.scaling);

    auto mesh = svc().resourceManager().getResource<MeshResource>(meshResourceId);
    if (!mesh)
    {
        DF3D_ASSERT(false);
        return nullptr;
    }

    switch (params.shape)
    {
    case CollisionShapeType::BOX:
    {
        auto half = (mesh->localAABB.maxPoint() - mesh->localAABB.minPoint()) / 2.0f;
        auto shape = MAKE_NEW(m_allocator, btBoxShape)(btVector3(half.x, half.y, half.z));
        shape->setLocalScaling(scale);
        return shape;
    }
    case CollisionShapeType::SPHERE:
    {
        auto shape = MAKE_NEW(m_allocator, btSphereShape)(mesh->localBoundingSphere.getRadius());
        shape->setLocalScaling(scale);

        return shape;
    }
    case CollisionShapeType::CONVEX_HULL:
    {
        auto &points = mesh->convexHull.m_vertices;
        auto colShape = MAKE_NEW(m_allocator, btConvexHullShape)((btScalar*)points.data(), points.size());
        colShape->setLocalScaling(scale);

        return colShape;
    }
    case CollisionShapeType::STATIC_MESH:
    {
        DF3D_ASSERT_MESS(params.mass == 0.0f, "body should not be dynamic");

        data.meshInterface = ShallowCopyBulletMeshData(mesh->physicsMeshInterface, m_allocator);

        auto colShape = MAKE_NEW(m_allocator, btBvhTriangleMeshShape)(data.meshInterface, true);
        colShape->setLocalScaling(scale);

        return colShape;
    }
    case CollisionShapeType::DYNAMIC_MESH:
    {
        data.meshInterface = ShallowCopyBulletMeshData(mesh->physicsMeshInterface, m_allocator);

        auto colShape = MAKE_NEW(m_allocator, btGImpactMeshShape)(data.meshInterface);
        colShape->setLocalScaling(scale);
        colShape->updateBound();

        return colShape;
    }
    break;
    default:
        DF3D_ASSERT_MESS(false, "undefined physics shape!");
        break;
    }

    return nullptr;
}

void PhysicsComponentProcessor::initialize(Data &data, df3d::Id meshResourceId, const PhysicsComponentCreationParams &params)
{
    btCollisionShape *colShape = createCollisionShape(data, meshResourceId, params);
    if (!colShape)
    {
        DF3D_ASSERT_MESS(false, "Failed to create a collision shape.");
        return;
    }

    btVector3 localInertia(0, 0, 0);
    if (!glm::epsilonEqual(params.mass, 0.0f, glm::epsilon<float>()))
        colShape->calculateLocalInertia(params.mass, localInertia);

    // Set motion state.
    auto myMotionState = createMotionState(data.holder);

    // Fill body properties.
    btRigidBody::btRigidBodyConstructionInfo rbInfo(params.mass, myMotionState, colShape, localInertia);
    rbInfo.m_friction = params.friction;
    rbInfo.m_restitution = params.restitution;
    rbInfo.m_linearDamping = params.linearDamping;
    rbInfo.m_angularDamping = params.angularDamping;

    data.body = createBody(rbInfo);

    addRigidBodyToWorld(data.body, params.groupId);

    if (params.disableDeactivation)
        data.body->setActivationState(DISABLE_DEACTIVATION);

    static_assert(sizeof(int) >= sizeof(Entity), "Can't store user data in bullet user data");

    data.body->setUserIndex(*reinterpret_cast<int*>(&data.holder));

    if (params.noContactResponse)
        data.body->setCollisionFlags(data.body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

void PhysicsComponentProcessor::update()
{
    m_dynamicsWorld->stepSimulation(svc().timer().getFrameDelta(TIME_CHANNEL_GAME), 10);
}

void PhysicsComponentProcessor::draw(RenderQueue *ops)
{
#ifdef DF3D_DESKTOP
#ifdef _DEBUG
    if (!EngineCVars::bulletDebugDraw)
        return;

    m_debugDraw->clean();

    // Collect render operations.
    m_dynamicsWorld->debugDrawWorld();
    // Append to render queue.
    m_debugDraw->flushRenderOperations(ops);
#endif
#endif
}

PhysicsComponentProcessor::PhysicsComponentProcessor(World &w)
    : m_df3dWorld(w),
    m_allocator(MemoryManager::allocDefault()),
    m_config(svc().getInitParams().physicsConfigPath)
{
    //btAlignedAllocSetCustom(CustomBulletAlloc, CustomBulletFree);

    m_collisionConfiguration = MAKE_NEW(m_allocator, btDefaultCollisionConfiguration)();
    m_dispatcher = MAKE_NEW(m_allocator, btCollisionDispatcher)(m_collisionConfiguration);
    m_overlappingPairCache = MAKE_NEW(m_allocator, btDbvtBroadphase)();
    m_solver = MAKE_NEW(m_allocator, btSequentialImpulseConstraintSolver)();
    m_dynamicsWorld = MAKE_NEW(m_allocator, btDiscreteDynamicsWorld)(m_dispatcher,
         m_overlappingPairCache,
         m_solver,
         m_collisionConfiguration);

    m_dynamicsWorld->setGravity(PhysicsHelpers::glmTobt(m_config.getGravity()));

    //btGImpactCollisionAlgorithm::registerAlgorithm(m_dispatcher);

#ifdef DF3D_DESKTOP
#ifdef _DEBUG
    m_debugDraw = MAKE_NEW(m_allocator, BulletDebugDraw)();
    m_dynamicsWorld->setDebugDrawer(m_debugDraw);
#endif
#endif

    m_data.setDestructionCallback([this](const Data &data) {
        if (data.body)
        {
            auto motionState = data.body->getMotionState();
            MAKE_DELETE(m_allocator, motionState);
            auto shape = data.body->getCollisionShape();
            MAKE_DELETE(m_allocator, shape);

            m_dynamicsWorld->removeRigidBody(data.body);
            MAKE_DELETE(m_allocator, data.body);
            MAKE_DELETE(m_allocator, data.meshInterface);
        }
    });
}

PhysicsComponentProcessor::~PhysicsComponentProcessor()
{
    // Calls destruction callback which does the deletion.
    m_data.clear();

    for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject *obj = m_dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody *body = btRigidBody::upcast(obj);
        if (auto motionState = body->getMotionState())
            MAKE_DELETE(m_allocator, motionState);
        if (auto shape = body->getCollisionShape())
            MAKE_DELETE(m_allocator, shape);

        m_dynamicsWorld->removeCollisionObject(obj);
        MAKE_DELETE(m_allocator, obj);
    }

    MAKE_DELETE(m_allocator, m_dynamicsWorld);
    MAKE_DELETE(m_allocator, m_solver);
    MAKE_DELETE(m_allocator, m_overlappingPairCache);
    MAKE_DELETE(m_allocator, m_dispatcher);
    MAKE_DELETE(m_allocator, m_collisionConfiguration);
#ifdef DF3D_DESKTOP
#ifdef _DEBUG
    MAKE_DELETE(m_allocator, m_debugDraw);
#endif
#endif
}

btRigidBody* PhysicsComponentProcessor::getBody(Entity e)
{
    return m_data.getData(e).body;
}

btRigidBody* PhysicsComponentProcessor::createBody(const btRigidBody::btRigidBodyConstructionInfo &info)
{
    // TODO: get from pool
    return MAKE_NEW(m_allocator, btRigidBody)(info);
}

btSphereShape* PhysicsComponentProcessor::createSphereShape(float raidus)
{
    return MAKE_NEW(m_allocator, btSphereShape)(raidus);
}

btCapsuleShape* PhysicsComponentProcessor::createCapsuleShape(float radius, float height, int axis)
{
    if (axis == 0)
        return MAKE_NEW(m_allocator, btCapsuleShapeX)(radius, height);
    else if (axis == 1)
        return MAKE_NEW(m_allocator, btCapsuleShape)(radius, height);
    else if (axis == 2)
        return MAKE_NEW(m_allocator, btCapsuleShapeZ)(radius, height);
    return nullptr;
}

btBoxShape* PhysicsComponentProcessor::createBoxShape(const glm::vec3 &halfSize)
{
    return MAKE_NEW(m_allocator, btBoxShape)(PhysicsHelpers::glmTobt(halfSize));
}

glm::vec3 PhysicsComponentProcessor::getCenterOfMass(Entity e)
{
    auto body = m_data.getData(e).body;
    return PhysicsHelpers::btToGlm(body->getCenterOfMassPosition());
}

const PhysicsComponentCreationParams* PhysicsComponentProcessor::getCreationParams(Entity e) const
{
    const auto &compData = m_data.getData(e);
    return compData.creationParams.get();
}

Id PhysicsComponentProcessor::getMeshResourceID(Entity e)
{
    const auto &compData = m_data.getData(e);
    return compData.meshResourceId;
}

void PhysicsComponentProcessor::teleportPosition(Entity e, const glm::vec3 &pos)
{
    auto body = getBody(e);
    if (body)
    {
        auto tr = body->getWorldTransform();
        tr.setOrigin(PhysicsHelpers::glmTobt(pos));

        body->setWorldTransform(tr);
        body->setInterpolationWorldTransform(tr);

        //m_dynamicsWorld->synchronizeSingleMotionState(body);
    }
}

void PhysicsComponentProcessor::teleportOrientation(Entity e, const glm::quat &orient)
{
    auto body = getBody(e);
    if (body)
    {
        auto tr = body->getWorldTransform();
        tr.setRotation(btQuaternion(orient.x, orient.y, orient.z, orient.w));

        body->setWorldTransform(tr);
        body->setInterpolationWorldTransform(tr);

        //m_dynamicsWorld->synchronizeSingleMotionState(body);
    }
}

void PhysicsComponentProcessor::add(Entity e, const PhysicsComponentCreationParams &params, Id meshResourceId)
{
    DF3D_ASSERT_MESS(!m_data.contains(e), "An entity already has a physics component");

    Data data;
    data.holder = e;

    initialize(data, meshResourceId, params);

    data.creationParams = make_shared<PhysicsComponentCreationParams>(params);
    data.meshResourceId = meshResourceId;

    m_data.add(e, data);
}

void PhysicsComponentProcessor::add(Entity e, btRigidBody *body, Id groupId)
{
    if (auto groupMask = m_config.getGroupMask(groupId))
        add(e, body, groupMask->first, groupMask->second);
    else
        DF3D_ASSERT(false);
}

void PhysicsComponentProcessor::add(Entity e, btRigidBody *body, short group, short mask)
{
    DF3D_ASSERT(body);

    if (m_data.contains(e))
    {
        DFLOG_WARN("An entity already has an physics component");
        return;
    }

    Data data;
    data.holder = e;
    data.body = body;

    addRigidBodyToWorld(body, group, mask);

    data.body->setUserIndex(*reinterpret_cast<int*>(&e));

    m_data.add(e, data);
}

void PhysicsComponentProcessor::remove(Entity e)
{
    m_data.remove(e);
}

bool PhysicsComponentProcessor::has(Entity e)
{
    return m_data.contains(e);
}

btDynamicsWorld* PhysicsComponentProcessor::getPhysicsWorld()
{
    return m_dynamicsWorld;
}

btMotionState* PhysicsComponentProcessor::createMotionState(Entity e)
{
    return MAKE_NEW(m_allocator, PhysicsComponentMotionState)(e, m_df3dWorld);
}

btMotionState* PhysicsComponentProcessor::createKinematicMotionState(Entity e)
{
    return MAKE_NEW(m_allocator, PhysicsComponentMotionStateKinematic)(e, m_df3dWorld);
}

}
