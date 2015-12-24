#pragma once

#include <game/Entity.h>
#include <game/EntityComponentProcessor.h>

class btRigidBody;
class btDynamicsWorld;

namespace df3d {

class RenderQueue;
class MeshData;
struct PhysicsComponentCreationParams;

class DF3D_DLL PhysicsComponentProcessor : public EntityComponentProcessor
{
    friend class World;

    struct Impl;
    unique_ptr<Impl> m_pimpl;

    void update() override;
    void draw(RenderQueue *ops);
    void cleanStep(const std::list<Entity> &deleted) override;

public:
    PhysicsComponentProcessor();
    ~PhysicsComponentProcessor();

    btRigidBody* getBody(Entity e);

    void teleport(Entity e, const glm::vec3 &pos);

    void add(Entity e, const PhysicsComponentCreationParams &params, shared_ptr<MeshData> meshData);
    void remove(Entity e);

    btDynamicsWorld* getPhysicsWorld();
};

}
