#include "df3d_pch.h"
#include "PhysicsComponent.h"

#include <BulletCollision/CollisionShapes/btShapeHull.h>

#include <scene/Node.h>
#include <components/MeshComponent.h>
#include <components/TransformComponent.h>
#include <components/serializers/PhysicsComponentSerializer.h>
#include <render/SubMesh.h>
#include <render/VertexIndexBuffer.h>
#include <render/MeshData.h>
#include <base/SystemsMacro.h>
#include <utils/Utils.h>

namespace df3d { namespace components {

void PhysicsComponent::initFromCreationParams()
{
    if (isInitialized())
        return;

    auto mesh = getHolder()->mesh();

    assert(mesh && mesh->isGeometryValid());

    btCollisionShape *colShape = nullptr;
    switch (m_creationParams.type)
    {
    case CollisionShapeType::BOX:
    {
        auto aabb = mesh->getAABB();
        if (!aabb.isValid())
        {
            base::glog << "Can not create box shape for rigid body. AABB is invalid" << base::logwarn;
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
            base::glog << "Can not create sphere shape for rigid body. Bounding sphere is invalid" << base::logwarn;
            return;
        }

        auto radius = sphere.getRadius();
        colShape = new btSphereShape(radius);
    }
        break;
    case CollisionShapeType::CONVEX_HULL:
    {
        auto convexHull = new btConvexHullShape();

        auto geometry = mesh->getGeometry();
        if (!geometry->valid())
        {
            base::glog << "Can not create convex hull for not valid geometry" << base::logwarn;
            return;
        }

        const auto &submeshes = geometry->getSubMeshes();
        for (auto &submesh : submeshes)
        {
            auto vb = submesh->getVertexBuffer();

            const float *vertexData = vb->getVertexData();
            size_t stride = vb->getFormat().getVertexSize() / sizeof(float);
            size_t posOffset = vb->getFormat().getOffsetTo(render::VertexComponent::POSITION) / sizeof(float);

            for (size_t vertex = 0; vertex < vb->getVerticesCount(); vertex++)
            {
                const float *base = vertexData + stride * vertex + posOffset;
                glm::vec3 p(base[0], base[1], base[2]);

                convexHull->addPoint(physics::glmTobt(p));
            }
        }

        // FIXME:
        // Too slow for big meshes. Can preload this from a special asset.
        // FIXME:
        // Use cache of already created hulls.
        // NOTE:
        // Save timestamp or save convex hull in a special binary format together with mesh.

        auto hull = new btShapeHull(convexHull);
        btScalar margin = convexHull->getMargin();
        hull->buildHull(margin);
        colShape = new btConvexHullShape((btScalar*)hull->getVertexPointer(), hull->numVertices());
        delete convexHull;

        colShape->setLocalScaling(physics::glmTobt(getHolder()->transform()->getScale()));
    }
        break;
    default:
        assert(false);
        return;
    }

    btVector3 localInertia(0, 0, 0);
    if (!glm::epsilonEqual(m_creationParams.mass, 0.0f, glm::epsilon<float>()))
        colShape->calculateLocalInertia(m_creationParams.mass, localInertia);

    // Set motion state.
    auto myMotionState = new physics::NodeMotionState(getHolder());

    // Fill body properties.
    btRigidBody::btRigidBodyConstructionInfo rbInfo(m_creationParams.mass, myMotionState, colShape, localInertia);
    rbInfo.m_friction = m_creationParams.friction;
    rbInfo.m_restitution = m_creationParams.restitution;
    rbInfo.m_linearDamping = m_creationParams.linearDamping;
    rbInfo.m_angularDamping = m_creationParams.angularDamping;

    body = new btRigidBody(rbInfo);

    if (m_creationParams.group != -1 && m_creationParams.mask != -1)
        g_physicsWorld->addRigidBody(body, m_creationParams.group, m_creationParams.mask);
    else
        g_physicsWorld->addRigidBody(body);

    // FIXME: remove this. Not needed.
    body->setUserPointer(m_holder);

    for (auto listener : m_listeners)
        listener->onPhysicsComponentInitialized();
}

void PhysicsComponent::onAttached()
{
    auto mesh = getHolder()->mesh();
    if (!mesh)
    {
        base::glog << "PhysicsComponent::onAttached failed: PhysicsComponent requires mesh component" << base::logwarn;
        return;
    }

    if (mesh->isGeometryValid())
        initFromCreationParams();
    // If geometry is not valid - wait till mesh being loaded.
}

void PhysicsComponent::onDetached()
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

void PhysicsComponent::onComponentEvent(ComponentEvent ev)
{
    if (ev == ComponentEvent::MESH_ASYNC_LOAD_COMPLETE)
        initFromCreationParams();
}

PhysicsComponent::PhysicsComponent(const CreationParams &params)
    : NodeComponent(PHYSICS),
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
        base::glog << "Trying to add duplicate PhysicsComponent listener" << base::logwarn;
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
        base::glog << "PhysicsComponent::removeListener failed: listener doesn't exist" << base::logwarn;
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
    tr.setOrigin(physics::glmTobt(pos));

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

} }
