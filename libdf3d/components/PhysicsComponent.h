#pragma once

#include "NodeComponent.h"

class btRigidBody;

namespace df3d {

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

    struct Listener
    {
        virtual ~Listener() { }

        virtual void onPhysicsComponentInitialized() { }
    };

private:
    CreationParams m_creationParams;

    std::vector<Listener*> m_listeners;

    void initFromCreationParams();

    void onAttached() override;
    void onDetached() override;
    void onComponentEvent(ComponentEvent ev) override;

public:
    btRigidBody *body = nullptr;

    // TODO: ctor with meshdata
    PhysicsComponent(const CreationParams &params);
    ~PhysicsComponent();

    bool isInitialized() const { return body != nullptr; }

    void addListener(Listener *listener);
    void removeListener(Listener *listener);

    shared_ptr<NodeComponent> clone() const override;

    // FIXME:
    // Call this from transformcomponent.
    void setPosition(const glm::vec3 &pos);
    void setOrientation(glm::vec3 rot, bool rads = false);
};

}
