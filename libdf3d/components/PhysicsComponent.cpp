#include "PhysicsComponent.h"

#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <physics/PhysicsHelpers.h>

#include <scene/Node.h>
#include <components/MeshComponent.h>
#include <components/TransformComponent.h>
#include <render/VertexIndexBuffer.h>
#include <render/MeshData.h>
#include <base/EngineController.h>
#include <utils/Utils.h>

namespace df3d {

void PhysicsComponent::initFromCreationParams()
{
    if (isInitialized())
        return;

    auto mesh = getHolder()->mesh();

    assert(mesh && mesh->getMeshData()->isInitialized());

    btCollisionShape *colShape = nullptr;
    switch (m_creationParams.type)
    {
    case CollisionShapeType::BOX:
    {
        auto aabb = mesh->getAABB();
        if (!aabb.isValid())
        {
            glog << "Can not create box shape for rigid body. AABB is invalid" << logwarn;
            return;
        }

        auto half = (aabb.maxPoint() - aabb.minPoint()) / 2.0f;
        colShape = new btBoxShape(btVector3(half.x, half.y, half.z));
    }
        break;
    case CollisionShapeType::SPHERE:
    {
        auto sphere = mesh->getBoundingSphere();
        if (!sphere.isValid())
        {
            glog << "Can not create sphere shape for rigid body. Bounding sphere is invalid" << logwarn;
            return;
        }

        auto radius = sphere.getRadius();
        colShape = new btSphereShape(radius);
    }
        break;
    case CollisionShapeType::CONVEX_HULL:
    {
        auto convexHull = mesh->getMeshData()->getConvexHull();
        if (!convexHull || !convexHull->isValid())
        {
            glog << "Can not create convex hull shape for rigid body. Hull is invalid" << logwarn;
            return;
        }

        const auto &vertices = convexHull->getVertices();
        std::vector<btVector3> tempPoints;
        for (const auto &v : vertices)
            tempPoints.push_back({ v.x, v.y, v.z });

        colShape = new btConvexHullShape((btScalar*)tempPoints.data(), tempPoints.size());

        // FIXME: what to do if scale has changed?
        colShape->setLocalScaling(glmTobt(getHolder()->transform()->getScale()));
    }
        break;
    default:
        assert(false);
        return;
    }

    btVector3 localInertia(0, 0, 0);
    if (!glm::epsilonEqual(m_creationParams.mass, 0.0f, glm::epsilon<float>()))
        colShape->calculateLocalInertia(m_creationParams.mass, localInertia);

    // TODO_ecs:
    assert(false);
    /*
    // Set motion state.
    auto myMotionState = new NodeMotionState(getHolder());

    // Fill body properties.
    btRigidBody::btRigidBodyConstructionInfo rbInfo(m_creationParams.mass, myMotionState, colShape, localInertia);
    rbInfo.m_friction = m_creationParams.friction;
    rbInfo.m_restitution = m_creationParams.restitution;
    rbInfo.m_linearDamping = m_creationParams.linearDamping;
    rbInfo.m_angularDamping = m_creationParams.angularDamping;

    body = new btRigidBody(rbInfo);

    if (m_creationParams.group != -1 && m_creationParams.mask != -1)
        svc().physicsManager().getWorld()->addRigidBody(body, m_creationParams.group, m_creationParams.mask);
    else
        svc().physicsManager().getWorld()->addRigidBody(body);

    // FIXME: remove this. Not needed.
    body->setUserPointer(m_holder);

    for (auto listener : m_listeners)
        listener->onPhysicsComponentInitialized();
        */
}

void PhysicsComponent::onAttached()
{
    auto mesh = getHolder()->mesh();
    if (!mesh)
    {
        glog << "PhysicsComponent::onAttached failed: PhysicsComponent requires mesh component" << logwarn;
        return;
    }

    if (mesh->getMeshData()->isInitialized())
        initFromCreationParams();
    // If geometry is not valid - wait till mesh being loaded.
}

void PhysicsComponent::onDetached()
{
    if (body)
    {
        // FIXME:
        // Here we assuming that body was added to the world.
        // TODO_ecs:
        assert(false);
        //svc().physicsManager().getWorld()->removeRigidBody(body);
        auto motionState = body->getMotionState();
        delete motionState;
        auto shape = body->getCollisionShape();
        delete shape;
        delete body;

        body = nullptr;
    }
}

void PhysicsComponent::onComponentEvent(ComponentEvent ev)
{
    if (ev == ComponentEvent::MESH_ASYNC_LOAD_COMPLETE)
        initFromCreationParams();
}

PhysicsComponent::PhysicsComponent(const CreationParams &params)
    : NodeComponent(ComponentType::PHYSICS),
    m_creationParams(params)
{

}

PhysicsComponent::~PhysicsComponent()
{
    onDetached();
}

void PhysicsComponent::addListener(Listener *listener)
{
    if (utils::contains(m_listeners, listener))
    {
        glog << "Trying to add duplicate PhysicsComponent listener" << logwarn;
        return;
    }

    m_listeners.push_back(listener);
}

void PhysicsComponent::removeListener(Listener *listener)
{
    auto found = std::find(m_listeners.begin(), m_listeners.end(), listener);
    if (found != m_listeners.end())
        m_listeners.erase(found);
    else
        glog << "PhysicsComponent::removeListener failed: listener doesn't exist" << logwarn;
}

shared_ptr<NodeComponent> PhysicsComponent::clone() const
{
    // TODO:
    assert(false);
    return nullptr;
}

void PhysicsComponent::setPosition(const glm::vec3 &pos)
{
    auto tr = body->getWorldTransform();
    tr.setOrigin(glmTobt(pos));

    body->setWorldTransform(tr);
}

void PhysicsComponent::setOrientation(glm::vec3 rot, bool rads)
{
    if (!rads)
        rot = glm::degrees(rot);

    auto tr = body->getWorldTransform();
    tr.setRotation(btQuaternion(rot.y, rot.x, rot.z));

    body->setWorldTransform(tr);
}

}
