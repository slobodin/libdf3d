#include "df3d_pch.h"
#include "PhysicsComponent.h"

#include <scene/Node.h>
#include <components/MeshComponent.h>
#include <components/TransformComponent.h>
#include <base/Controller.h>
#include <physics/PhysicsManager.h>
#include <utils/PhysicsLoader.h>

namespace df3d { namespace components {


// TODO:
//void PhysicsComponent::onUpdate(float dt)
//{
//    // FIXME:
//    // Because async.
//    if (!body && m_meshLoaded)
//    {
//        
//    }
//}

void PhysicsComponent::onAttached()
{
    utils::physics_loader::init(this, m_definitionFile.c_str());
}

PhysicsComponent::PhysicsComponent(const char *definitionFile)
    : NodeComponent(CT_PHYSICS),
    m_definitionFile(definitionFile)
{

}

PhysicsComponent::~PhysicsComponent()
{
    if (body)
    {
        // FIXME:
        // Here we assuming that body was added to the world.
        // TODO: Add physics body when create this component. But how to pass collision idx?
        g_physicsWorld->removeRigidBody(body);
        auto motionState = body->getMotionState();
        delete motionState;
        auto shape = body->getCollisionShape();
        delete shape;
        delete body;

        body = nullptr;
    }
}

shared_ptr<NodeComponent> PhysicsComponent::clone() const
{
    // TODO:
    assert(false);
    return nullptr;
}

} }