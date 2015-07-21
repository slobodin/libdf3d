#pragma once

#include "NodeComponent.h"

class btRigidBody;

namespace df3d { namespace components {

class DF3D_DLL PhysicsComponent : public NodeComponent
{
public:
    enum class CollisionShapeType
    {
        BOX,
        SPHERE,
        CONVEX_HULL
    };

    struct CreationParams
    {
        CollisionShapeType type;

        float mass = 1.0f;
        float friction = 0.5f;
        float restitution = 0.0f;
        float linearDamping = 0.0f;
        float angularDamping = 0.0f;
        short group = -1;
        short mask = -1;
    };

private:
    CreationParams m_creationParams;

    void initFromCreationParams();

    //void onUpdate(float dt);
    void onAttached() override;
    void onDetached() override;

public:
    btRigidBody *body = nullptr;

    PhysicsComponent(const CreationParams &params);
    ~PhysicsComponent();

    shared_ptr<NodeComponent> clone() const override;

    // FIXME:
    // Call this from transformcomponent.
    void setPosition(const glm::vec3 &pos);
    void setOrientation(glm::vec3 rot, bool rads = false);
};

} }
