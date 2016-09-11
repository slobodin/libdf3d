#include "PhysicsComponentProcessor.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "BulletInterface.h"
#include "PhysicsComponentCreationParams.h"
#include "PhysicsHelpers.h"
#include <df3d/lib/math/AABB.h>
#include <df3d/lib/math/BoundingSphere.h>
#include <df3d/lib/math/ConvexHull.h>
#include <df3d/engine/render/MeshData.h>
#include <df3d/engine/3d/StaticMeshComponentProcessor.h>
#include <df3d/engine/3d/SceneGraphComponentProcessor.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/EngineCVars.h>
#include <df3d/engine/TimeManager.h>
#include <df3d/game/ComponentDataHolder.h>
#include <df3d/game/World.h>
#include <df3d/engine/io/FileSystem.h>
#include <df3d/engine/resources/MeshLoaders.h>

namespace df3d {

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

struct PhysicsComponentProcessor::Impl
{
    World &df3dWorld;

    btDefaultCollisionConfiguration *collisionConfiguration = nullptr;
    btCollisionDispatcher *dispatcher = nullptr;
    btBroadphaseInterface *overlappingPairCache = nullptr;
    btSequentialImpulseConstraintSolver *solver = nullptr;
    btDiscreteDynamicsWorld *dynamicsWorld = nullptr;

    physics_impl::BulletDebugDraw *debugDraw;

    struct Data
    {
        weak_ptr<MeshData> mesh;
        shared_ptr<PhysicsComponentCreationParams> params;
        Entity holder;
        btRigidBody *body = nullptr;
        bool initialized = false;

        btStridingMeshInterface *meshInterface = nullptr;
    };

    ComponentDataHolder<Data> data;

    Impl(World &w)
        : df3dWorld(w)
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
        // Calls destruction callback which does the deletion.
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

    btCollisionShape* createCollisionShape(Data &data)
    {
        switch (data.params->shape)
        {
        case CollisionShapeType::BOX:
        {
            auto aabb = svc().defaultWorld().staticMesh().getAABB(data.holder);
            if (!aabb.isValid())
            {
                DFLOG_WARN("Can not create box shape for rigid body. AABB is invalid");
                return nullptr;
            }

            auto half = (aabb.maxPoint() - aabb.minPoint()) / 2.0f;
            return new btBoxShape(btVector3(half.x, half.y, half.z));
        }
        case CollisionShapeType::SPHERE:
        {
            auto sphere = svc().defaultWorld().staticMesh().getBoundingSphere(data.holder);
            if (!sphere.isValid())
            {
                DFLOG_WARN("Can not create sphere shape for rigid body. Bounding sphere is invalid");
                return  nullptr;
            }

            auto radius = sphere.getRadius();
            return new btSphereShape(radius);
        }
        case CollisionShapeType::CONVEX_HULL:
        {
            auto convexHull = data.mesh.lock()->getConvexHull();
            if (!convexHull || !convexHull->isValid())
            {
                DFLOG_WARN("Can not create convex hull shape for rigid body. Hull is invalid");
                return nullptr;
            }

            const auto &vertices = convexHull->getVertices();
            PodArray<btVector3> tempPoints(MemoryManager::allocDefault());
            tempPoints.resize(vertices.size());

            for (size_t i = 0; i < vertices.size(); i++)
                tempPoints[i] = PhysicsHelpers::glmTobt(vertices[i]);

            auto colShape = new btConvexHullShape((btScalar*)tempPoints.data(), tempPoints.size());

            // FIXME: what to do if scale has been changed?
            auto scale = svc().defaultWorld().sceneGraph().getLocalScale(data.holder);
            colShape->setLocalScaling(PhysicsHelpers::glmTobt(scale));

            return colShape;
        }
        case CollisionShapeType::STATIC_TRIANGLE_MESH:
        {
            // FIXME: XXX! Reading the mesh twice! This is a workaround.
            auto meshFileData = svc().fileSystem().open(data.mesh.lock()->getFilePath().c_str());
            auto softwareMesh = LoadMeshDataFromFile_Workaround(meshFileData);

            DF3D_ASSERT_MESS(data.params->mass == 0.0f, "body should not be dynamic");

            auto bulletMesh = new btTriangleMesh(true, false);

            for (size_t smIdx = 0; smIdx < softwareMesh->submeshes.size(); smIdx++)
            {
                auto &submesh = softwareMesh->submeshes[smIdx];
                auto &vdata = submesh.vertexData;

                bulletMesh->preallocateVertices(vdata.getVerticesCount());

                if (submesh.indices.size() > 0)
                {
                    for (size_t i = 0; i < submesh.indices.size(); i += 3)
                    {
                        auto i1 = submesh.indices[i + 0];
                        auto i2 = submesh.indices[i + 1];
                        auto i3 = submesh.indices[i + 2];

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

            data.meshInterface = bulletMesh;

            auto colShape = new btBvhTriangleMeshShape(bulletMesh, true);
            auto scale = svc().defaultWorld().sceneGraph().getLocalScale(data.holder);
            colShape->setLocalScaling(PhysicsHelpers::glmTobt(scale));

            return colShape;
        }
        default:
            DF3D_ASSERT_MESS(false, "undefined physics shape!");
        }

        return nullptr;
    }

    void initialize(Data &data)
    {
        DF3D_ASSERT_MESS(!data.initialized, "physics body already initialized");

        btCollisionShape *colShape = createCollisionShape(data);
        if (!colShape)
        {
            DFLOG_WARN("Failed to create a collision shape.");
            data.initialized = true;
            data.params.reset();
            return;
        }

        btVector3 localInertia(0, 0, 0);
        if (!glm::epsilonEqual(data.params->mass, 0.0f, glm::epsilon<float>()))
            colShape->calculateLocalInertia(data.params->mass, localInertia);

        // Set motion state.
        auto myMotionState = createMotionState(data.holder);

        // Fill body properties.
        btRigidBody::btRigidBodyConstructionInfo rbInfo(data.params->mass, myMotionState, colShape, localInertia);
        rbInfo.m_friction = data.params->friction;
        rbInfo.m_restitution = data.params->restitution;
        rbInfo.m_linearDamping = data.params->linearDamping;
        rbInfo.m_angularDamping = data.params->angularDamping;

        data.body = new btRigidBody(rbInfo);

        if (data.params->group != -1 && data.params->mask != -1)
            dynamicsWorld->addRigidBody(data.body, data.params->group, data.params->mask);
        else
            dynamicsWorld->addRigidBody(data.body);

        if (data.params->disableDeactivation)
            data.body->setActivationState(DISABLE_DEACTIVATION);

        static_assert(sizeof(int) >= sizeof(Entity), "Can't store user data in bullet user data");

        data.body->setUserIndex(*reinterpret_cast<int*>(&data.holder));

        if (data.params->noContactResponse)
            data.body->setCollisionFlags(data.body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

        data.initialized = true;
        data.params.reset();
    }

    btMotionState* createMotionState(Entity e)
    {
        return new PhysicsComponentMotionState(e, df3dWorld);
    }
};

void PhysicsComponentProcessor::update()
{
    for (auto &data : m_pimpl->data.rawData())
    {
        if (!data.initialized)
        {
            if (data.mesh.lock()->isInitialized())
                m_pimpl->initialize(data);
        }
    }

    m_pimpl->dynamicsWorld->stepSimulation(svc().timer().getFrameDelta(TIME_CHANNEL_GAME), 10);
}

void PhysicsComponentProcessor::draw(RenderQueue *ops)
{
    if (!EngineCVars::bulletDebugDraw)
        return;

    m_pimpl->debugDraw->clean();

    // Collect render operations.
    m_pimpl->dynamicsWorld->debugDrawWorld();
    // Append to render queue.
    m_pimpl->debugDraw->flushRenderOperations(ops);
}

PhysicsComponentProcessor::PhysicsComponentProcessor(World *w)
    : m_pimpl(new Impl(*w))
{
    auto physicsWorld = m_pimpl->dynamicsWorld;
    m_pimpl->data.setDestructionCallback([this, physicsWorld](const Impl::Data &data) {
        if (data.body)
        {
            auto motionState = data.body->getMotionState();
            delete motionState;
            auto shape = data.body->getCollisionShape();
            delete shape;

            physicsWorld->removeRigidBody(data.body);
            delete data.body;
            delete data.meshInterface;
        }
    });
}

PhysicsComponentProcessor::~PhysicsComponentProcessor()
{

}

btRigidBody* PhysicsComponentProcessor::getBody(Entity e)
{
    return m_pimpl->data.getData(e.handle).body;
}

glm::vec3 PhysicsComponentProcessor::getCenterOfMass(Entity e)
{
    auto body = m_pimpl->data.getData(e.handle).body;
    DF3D_ASSERT(body);
    return PhysicsHelpers::btToGlm(body->getCenterOfMassPosition());
}

void PhysicsComponentProcessor::teleportPosition(Entity e, const glm::vec3 &pos)
{
    auto body = getBody(e);
    if (body)
    {
        auto tr = body->getWorldTransform();
        tr.setOrigin(PhysicsHelpers::glmTobt(pos));

        body->setWorldTransform(tr);

        m_pimpl->dynamicsWorld->synchronizeSingleMotionState(body);
    }
    else
    {
        m_pimpl->df3dWorld.sceneGraph().setPosition(e, pos);
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

        m_pimpl->dynamicsWorld->synchronizeSingleMotionState(body);
    }
    else
    {
        m_pimpl->df3dWorld.sceneGraph().setOrientation(e, orient);
    }
}

void PhysicsComponentProcessor::add(Entity e, const PhysicsComponentCreationParams &params, shared_ptr<MeshData> mesh)
{
    if (m_pimpl->data.contains(e.handle))
    {
        DFLOG_WARN("An entity already has an physics component");
        return;
    }

    Impl::Data data;
    data.mesh = mesh;
    data.holder = e;
    data.params = make_shared<PhysicsComponentCreationParams>(params);

    if (mesh->isInitialized())
        m_pimpl->initialize(data);

    m_pimpl->data.add(e.handle, data);
}

void PhysicsComponentProcessor::add(Entity e, btRigidBody *body, short group, short mask)
{
    DF3D_ASSERT(body);

    if (m_pimpl->data.contains(e.handle))
    {
        DFLOG_WARN("An entity already has an physics component");
        return;
    }

    Impl::Data data;
    data.holder = e;
    data.body = body;
    data.initialized = true;

    if (group != -1 && mask != -1)
        m_pimpl->dynamicsWorld->addRigidBody(body, group, mask);
    else
        m_pimpl->dynamicsWorld->addRigidBody(body);

    data.body->setUserIndex(*reinterpret_cast<int*>(&e));

    m_pimpl->data.add(e.handle, data);
}

void PhysicsComponentProcessor::remove(Entity e)
{
    m_pimpl->data.remove(e.handle);
}

bool PhysicsComponentProcessor::has(Entity e)
{
    return m_pimpl->data.contains(e.handle);
}

btDynamicsWorld* PhysicsComponentProcessor::getPhysicsWorld()
{
    return m_pimpl->dynamicsWorld;
}

btMotionState* PhysicsComponentProcessor::createMotionState(Entity e)
{
    return m_pimpl->createMotionState(e);
}

}
