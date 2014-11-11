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

    if (m_group != -1 && m_mask != -1)
        g_physicsWorld->addRigidBody(body, m_group, m_mask);
    else 
        g_physicsWorld->addRigidBody(body);
}

PhysicsComponent::PhysicsComponent(const char *definitionFile, short group, short mask)
    : NodeComponent(ComponentType::PHYSICS),
    m_definitionFile(definitionFile),
    m_group(group),
    m_mask(mask)
{

}

PhysicsComponent::~PhysicsComponent()
{
    if (body)
    {
        // FIXME:
        // Here we assuming that body was added to the world.
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