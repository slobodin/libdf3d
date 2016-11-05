#pragma once

#include <df3d/game/Entity.h>
#include <df3d/game/EntityComponentProcessor.h>
#include <df3d/game/ComponentDataHolder.h>
#include <third-party/bullet/src/BulletDynamics/Dynamics/btRigidBody.h>

class btDynamicsWorld;
class btMotionState;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btStridingMeshInterface;
class btCollisionShape;
class btSphereShape;
class btBoxShape;

namespace df3d {

class RenderQueue;
class World;
struct PhysicsComponentCreationParams;
class BulletDebugDraw;

class PhysicsComponentProcessor : public EntityComponentProcessor
{
    World &m_df3dWorld;
    Allocator &m_allocator;

    btDefaultCollisionConfiguration *m_collisionConfiguration = nullptr;
    btCollisionDispatcher *m_dispatcher = nullptr;
    btBroadphaseInterface *m_overlappingPairCache = nullptr;
    btSequentialImpulseConstraintSolver *m_solver = nullptr;
    btDiscreteDynamicsWorld *m_dynamicsWorld = nullptr;

    BulletDebugDraw *m_debugDraw = nullptr;

    struct Data
    {
        Entity holder;
        btRigidBody *body = nullptr;
        btStridingMeshInterface *meshInterface = nullptr;
    };

    ComponentDataHolder<Data> m_data;

    btCollisionShape* createCollisionShape(Data &data, const ResourceID &meshResourceID, const PhysicsComponentCreationParams &params);
    void initialize(Data &data, const ResourceID &meshResourceID, const PhysicsComponentCreationParams &params);
    void update() override;
    void draw(RenderQueue *ops) override;

public:
    PhysicsComponentProcessor(World &w);
    ~PhysicsComponentProcessor();

    btRigidBody* getBody(Entity e);
    btRigidBody* createBody(const btRigidBody::btRigidBodyConstructionInfo &info);
    btSphereShape* createSphereShape(float raidus);
    btBoxShape* createBoxShape(const glm::vec3 &halfSize);
    glm::vec3 getCenterOfMass(Entity e);

    void teleportPosition(Entity e, const glm::vec3 &pos);
    void teleportOrientation(Entity e, const glm::quat &orient);

    void add(Entity e, const PhysicsComponentCreationParams &params, const ResourceID &meshResource);
    // NOTE: body should not be added to the Physics World as it will be added via this processor.
    void add(Entity e, btRigidBody *body, short group = -1, short mask = -1);

    void remove(Entity e) override;
    bool has(Entity e) override;

    btDynamicsWorld* getPhysicsWorld();
    btMotionState* createMotionState(Entity e);
};

}
