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
#include <df3d/engine/DebugConsole.h>
#include <df3d/engine/TimeManager.h>
#include <df3d/game/ComponentDataHolder.h>
#include <df3d/game/World.h>
#include <df3d/engine/io/DefaultFileSystem.h>
#include <df3d/engine/resources/MeshLoaders.h>

namespace df3d {

static_assert(sizeof(int) >= sizeof(decltype(df3d::Entity::id)), "Can't store user data in bullet user pointer");

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

        m_transform = btTransform(glmTobt(orientation), glmTobt(position));
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
                tempPoints[i] = glmTobt(vertices[i]);

            auto colShape = new btConvexHullShape((btScalar*)tempPoints.data(), tempPoints.size());

            // FIXME: what to do if scale has been changed?
            auto scale = svc().defaultWorld().sceneGraph().getLocalScale(data.holder);
            colShape->setLocalScaling(glmTobt(scale));

            return colShape;
        }
        case CollisionShapeType::STATIC_TRIANGLE_MESH:
        {
            // FIXME: XXX! Reading the mesh twice! This is a workaround.
            auto meshFileData = svc().fileSystem().open(data.mesh.lock()->getFilePath());
            auto softwareMesh = LoadMeshDataFromFile_Workaround(meshFileData);

            DF3D_ASSERT_MESS(data.params->mass == 0.0f, "body should not be dynamic");

            auto bulletMesh = new btTriangleMesh(true, false);

            for (size_t smIdx = 0; smIdx < softwareMesh->submeshes.size(); smIdx++)
            {
                auto &submesh = softwareMesh->submeshes[smIdx];

                if (submesh.hasIndices())
                {
                    DF3D_ASSERT_MESS(false, "not implemented");
                }
                else
                {
                    for (size_t i = 0; i < submesh.getVertexData().getVerticesCount(); i += 3)
                    {
                        auto v1 = submesh.getVertexData().getVertex(i + 0);
                        auto v2 = submesh.getVertexData().getVertex(i + 1);
                        auto v3 = submesh.getVertexData().getVertex(i + 2);

                        glm::vec3 p1, p2, p3;
                        v1.getPosition(&p1);
                        v2.getPosition(&p2);
                        v3.getPosition(&p3);

                        bulletMesh->addTriangle(glmTobt(p1), glmTobt(p2), glmTobt(p3));
                    }
                }
            }

            data.meshInterface = bulletMesh;

            auto colShape = new btBvhTriangleMeshShape(bulletMesh, true);
            auto scale = svc().defaultWorld().sceneGraph().getLocalScale(data.holder);
            colShape->setLocalScaling(glmTobt(scale));

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

        data.body->setUserIndex(data.holder.id);

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

void PhysicsComponentProcessor::cleanStep(const std::list<Entity> &deleted)
{
    m_pimpl->data.cleanStep(deleted);
    m_pimpl->debugDraw->clean();
}

void PhysicsComponentProcessor::draw(RenderQueue *ops)
{
    if (auto console = svc().debugConsole())
    {
        if (!console->getCVars().get<bool>(CVAR_DEBUG_DRAW))
            return;
    }

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
    return m_pimpl->data.getData(e).body;
}

glm::vec3 PhysicsComponentProcessor::getCenterOfMass(Entity e)
{
    auto body = m_pimpl->data.getData(e).body;
    DF3D_ASSERT(body);
    return btToGlm(body->getCenterOfMassPosition());
}

void PhysicsComponentProcessor::teleportPosition(Entity e, const glm::vec3 &pos)
{
    auto body = getBody(e);
    if (body)
    {
        auto tr = body->getWorldTransform();
        tr.setOrigin(glmTobt(pos));

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
    if (m_pimpl->data.contains(e))
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

    m_pimpl->data.add(e, data);
}

void PhysicsComponentProcessor::add(Entity e, btRigidBody *body, short group, short mask)
{
    DF3D_ASSERT(body);

    if (m_pimpl->data.contains(e))
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

    data.body->setUserIndex(e.id);

    m_pimpl->data.add(e, data);
}

void PhysicsComponentProcessor::remove(Entity e)
{
    if (!m_pimpl->data.contains(e))
    {
        DFLOG_WARN("Failed to remove physics component from an entity. Component is not attached");
        return;
    }

    m_pimpl->data.remove(e);
}

bool PhysicsComponentProcessor::has(Entity e)
{
    return m_pimpl->data.lookup(e).valid();
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
