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
class btCapsuleShape;

namespace df3d {

struct RenderQueue;
class World;
struct PhysicsComponentCreationParams;
class BulletDebugDraw;

class PhysicsConfig
{
    glm::vec3 m_gravity;
    std::unordered_map<Id, std::pair<short, short>> m_collisionGroups;

public:
    PhysicsConfig(const std::string &physicsConfigPath);
    const glm::vec3& getGravity() const { return m_gravity; }

    const std::pair<short, short>* getGroupMask(Id groupId) const;
};

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
    PhysicsConfig m_config;

    void addRigidBodyToWorld(btRigidBody *body, Id groupId);
    void addRigidBodyToWorld(btRigidBody *body, short group, short mask);
    btCollisionShape* createCollisionShape(Data &data, df3d::Id meshResourceId, const PhysicsComponentCreationParams &params);
    void initialize(Data &data, df3d::Id meshResourceId, const PhysicsComponentCreationParams &params);
    void update() override;
    void draw(RenderQueue *ops) override;

public:
    PhysicsComponentProcessor(World &w);
    ~PhysicsComponentProcessor();

    const PhysicsConfig& getConfig() const { return m_config; }

    btRigidBody* getBody(Entity e);
    btRigidBody* createBody(const btRigidBody::btRigidBodyConstructionInfo &info);
    btSphereShape* createSphereShape(float raidus);
    // axisX -> 0, axis y -> 1, axis z -> 2
    btCapsuleShape* createCapsuleShape(float radius, float height, int axis);
    btBoxShape* createBoxShape(const glm::vec3 &halfSize);
    glm::vec3 getCenterOfMass(Entity e);

    void teleportPosition(Entity e, const glm::vec3 &pos);
    void teleportOrientation(Entity e, const glm::quat &orient);

    void add(Entity e, const PhysicsComponentCreationParams &params, df3d::Id meshResourceId);
    // NOTE: body should not be added to the Physics World as it will be added via this processor.
    void add(Entity e, btRigidBody *body, Id groupId);
    void add(Entity e, btRigidBody *body, short group, short mask);

    void remove(Entity e) override;
    bool has(Entity e) override;

    btDynamicsWorld* getPhysicsWorld();
    btMotionState* createMotionState(Entity e);
    btMotionState* createKinematicMotionState(Entity e);
};

}
