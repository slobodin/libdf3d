#include "PhysicsComponentProcessor.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <ConvexDecomposition/ConvexDecomposition.h>
#include "BulletInterface.h"
#include "btGImpactConvexDecompositionShape.h"
#include "PhysicsComponentCreationParams.h"
#include "PhysicsHelpers.h"
#include <df3d/lib/math/AABB.h>
#include <df3d/lib/math/BoundingSphere.h>
#include <df3d/engine/3d/SceneGraphComponentProcessor.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/EngineCVars.h>
#include <df3d/engine/TimeManager.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/MeshResource.h>
#include <df3d/game/ComponentDataHolder.h>
#include <df3d/game/World.h>

namespace df3d {

static btTriangleMesh* CreateBulletTriangleMesh(const std::string &meshPath, Allocator &allocator)
{
    // FIXME: XXX! Reading the mesh twice! This is a workaround.
    auto meshFileData = LoadMeshDataFromFile_Workaround(meshPath, allocator);

    auto bulletMesh = MAKE_NEW(allocator, btTriangleMesh)(true, false);

    for (size_t smIdx = 0; smIdx < meshFileData->parts.size(); smIdx++)
    {
        auto &submesh = meshFileData->parts[smIdx];
        auto &vdata = submesh->vertexData;

        bulletMesh->preallocateVertices(vdata.getVerticesCount());

        if (submesh->indices.size() > 0)
        {
            for (size_t i = 0; i < submesh->indices.size(); i += 3)
            {
                auto i1 = submesh->indices[i + 0];
                auto i2 = submesh->indices[i + 1];
                auto i3 = submesh->indices[i + 2];

                auto v1 = (glm::vec3*)vdata.getVertexAttribute(i1, VertexFormat::POSITION);
                auto v2 = (glm::vec3*)vdata.getVertexAttribute(i2, VertexFormat::POSITION);
                auto v3 = (glm::vec3*)vdata.getVertexAttribute(i3, VertexFormat::POSITION);

                bulletMesh->addTriangle(PhysicsHelpers::glmTobt(*v1), PhysicsHelpers::glmTobt(*v2), PhysicsHelpers::glmTobt(*v3));
            }
        }
        else
        {
            for (size_t i = 0; i < vdata.getVerticesCount(); i += 3)
            {
                auto v1 = (glm::vec3*)vdata.getVertexAttribute(i + 0, VertexFormat::POSITION);
                auto v2 = (glm::vec3*)vdata.getVertexAttribute(i + 1, VertexFormat::POSITION);
                auto v3 = (glm::vec3*)vdata.getVertexAttribute(i + 2, VertexFormat::POSITION);

                bulletMesh->addTriangle(PhysicsHelpers::glmTobt(*v1), PhysicsHelpers::glmTobt(*v2), PhysicsHelpers::glmTobt(*v3));
            }
        }
    }

    DestroyMeshData(meshFileData, allocator);

    return bulletMesh;
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

    void getWorldTransform(btTransform &worldTrans) const
    {
        worldTrans = m_transform;
    }

    void setWorldTransform(const btTransform &worldTrans)
    {
        m_transform = worldTrans;
        m_world.sceneGraph().setWorldTransform(m_holder, worldTrans);
    }
};

btCollisionShape* PhysicsComponentProcessor::createCollisionShape(Data &data, const ResourceID &meshResourceID, const PhysicsComponentCreationParams &params)
{
    // FIXME: what to do if scale has been changed?
    auto scale = PhysicsHelpers::glmTobt(svc().defaultWorld().sceneGraph().getLocalScale(data.holder));

    switch (params.shape)
    {
    case CollisionShapeType::BOX:
    {
        auto mesh = svc().resourceManager().getResource<MeshResource>(meshResourceID);
        DF3D_ASSERT(mesh);

        auto half = (mesh->localAABB.maxPoint() - mesh->localAABB.minPoint()) / 2.0f;
        auto shape = MAKE_NEW(m_allocator, btBoxShape)(btVector3(half.x, half.y, half.z));
        shape->setLocalScaling(scale);
        return shape;
    }
    case CollisionShapeType::SPHERE:
    {
        auto mesh = svc().resourceManager().getResource<MeshResource>(meshResourceID);
        DF3D_ASSERT(mesh);

        auto shape = MAKE_NEW(m_allocator, btSphereShape)(mesh->localBoundingSphere.getRadius());
        shape->setLocalScaling(scale);

        return shape;
    }
    case CollisionShapeType::CONVEX_HULL:
    {
        auto mesh = svc().resourceManager().getResource<MeshResource>(meshResourceID);
        DF3D_ASSERT(mesh);

        auto &points = mesh->convexHull.m_vertices;
        auto colShape = MAKE_NEW(m_allocator, btConvexHullShape)((btScalar*)points.data(), points.size());
        colShape->setLocalScaling(scale);

        return colShape;
    }
    case CollisionShapeType::STATIC_MESH:
    {
        DF3D_ASSERT_MESS(params.mass == 0.0f, "body should not be dynamic");

        data.meshInterface = CreateBulletTriangleMesh(meshResourceID, m_allocator);

        auto colShape = MAKE_NEW(m_allocator, btBvhTriangleMeshShape)(data.meshInterface, true);
        colShape->setLocalScaling(scale);

        return colShape;
    }
    case CollisionShapeType::CONVEX_DECOMPOSITION:
    {
        data.meshInterface = CreateBulletTriangleMesh(meshResourceID, m_allocator);

        auto colShape = MAKE_NEW(m_allocator, btGImpactConvexDecompositionShape)(data.meshInterface, scale);

        colShape->setMargin(0.07f);
        colShape->updateBound();
        return colShape;
    }
    case CollisionShapeType::DYNAMIC_MESH:
    {
        data.meshInterface = CreateBulletTriangleMesh(meshResourceID, m_allocator);
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

void PhysicsComponentProcessor::initialize(Data &data, const ResourceID &meshResourceID, const PhysicsComponentCreationParams &params)
{
    btCollisionShape *colShape = createCollisionShape(data, meshResourceID, params);
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

    if (params.group != -1 && params.mask != -1)
        m_dynamicsWorld->addRigidBody(data.body, params.group, params.mask);
    else
        m_dynamicsWorld->addRigidBody(data.body);

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
#ifdef _DEBUG
    if (!EngineCVars::bulletDebugDraw)
        return;

    m_debugDraw->clean();

    // Collect render operations.
    m_dynamicsWorld->debugDrawWorld();
    // Append to render queue.
    m_debugDraw->flushRenderOperations(ops);
#endif
}

PhysicsComponentProcessor::PhysicsComponentProcessor(World &w)
    : m_df3dWorld(w),
    m_allocator(MemoryManager::allocDefault())
{
    m_collisionConfiguration = MAKE_NEW(m_allocator, btDefaultCollisionConfiguration)();
    m_dispatcher = MAKE_NEW(m_allocator, btCollisionDispatcher)(m_collisionConfiguration);
    m_overlappingPairCache = MAKE_NEW(m_allocator, btDbvtBroadphase)();
    m_solver = MAKE_NEW(m_allocator, btSequentialImpulseConstraintSolver)();
    m_dynamicsWorld = MAKE_NEW(m_allocator, btDiscreteDynamicsWorld)(m_dispatcher,
         m_overlappingPairCache,
         m_solver,
         m_collisionConfiguration);

    m_dynamicsWorld->setGravity(btVector3(0, 0, 0));

#ifdef _DEBUG
    m_debugDraw = MAKE_NEW(m_allocator, BulletDebugDraw)();
    m_dynamicsWorld->setDebugDrawer(m_debugDraw);
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
#ifdef _DEBUG
    MAKE_DELETE(m_allocator, m_debugDraw);
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

btBoxShape* PhysicsComponentProcessor::createBoxShape(const glm::vec3 &halfSize)
{
    return MAKE_NEW(m_allocator, btBoxShape)(PhysicsHelpers::glmTobt(halfSize));
}

glm::vec3 PhysicsComponentProcessor::getCenterOfMass(Entity e)
{
    auto body = m_data.getData(e).body;
    return PhysicsHelpers::btToGlm(body->getCenterOfMassPosition());
}

void PhysicsComponentProcessor::teleportPosition(Entity e, const glm::vec3 &pos)
{
    auto body = getBody(e);
    auto tr = body->getWorldTransform();
    tr.setOrigin(PhysicsHelpers::glmTobt(pos));

    body->setWorldTransform(tr);

    m_dynamicsWorld->synchronizeSingleMotionState(body);
}

void PhysicsComponentProcessor::teleportOrientation(Entity e, const glm::quat &orient)
{
    auto body = getBody(e);
    auto tr = body->getWorldTransform();
    tr.setRotation(btQuaternion(orient.x, orient.y, orient.z, orient.w));

    body->setWorldTransform(tr);

    m_dynamicsWorld->synchronizeSingleMotionState(body);
}

void PhysicsComponentProcessor::add(Entity e, const PhysicsComponentCreationParams &params, const ResourceID &meshResource)
{
    if (m_data.contains(e))
    {
        DFLOG_WARN("An entity already has an physics component");
        return;
    }

    Data data;
    data.holder = e;
    initialize(data, meshResource, params);
    m_data.add(e, data);
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

    if (group != -1 && mask != -1)
        m_dynamicsWorld->addRigidBody(body, group, mask);
    else
        m_dynamicsWorld->addRigidBody(body);

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

}
