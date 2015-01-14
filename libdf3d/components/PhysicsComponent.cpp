#include "df3d_pch.h"
#include "PhysicsComponent.h"

#include <scene/Node.h>
#include <components/MeshComponent.h>
#include <components/TransformComponent.h>
#include <base/Controller.h>
#include <physics/PhysicsManager.h>
#include <components/serializers/PhysicsComponentSerializer.h>
#include <render/SubMesh.h>
#include <render/VertexIndexBuffer.h>
#include <render/MeshData.h>

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

void PhysicsComponent::initFromCreationParams()
{
    auto mesh = getHolder()->mesh();
    if (!mesh || !mesh->isGeometryValid())
    {
        base::glog << "Can not create rigid body. Node doesn't have mesh component or it's invalid." << base::logwarn;
        // TODO:
        assert(false);
        return;
    }

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
        colShape = new btConvexHullShape();

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

                ((btConvexHullShape *)colShape)->addPoint(physics::glmTobt(p));
            }
        }

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
}

void PhysicsComponent::onAttached()
{
    initFromCreationParams();

    if (m_creationParams.group != -1 && m_creationParams.mask != -1)
        g_physicsWorld->addRigidBody(body, m_creationParams.group, m_creationParams.mask);
    else 
        g_physicsWorld->addRigidBody(body);

    body->setUserPointer(m_holder);
}

PhysicsComponent::PhysicsComponent(const CreationParams &params)
    : NodeComponent(PHYSICS),
    m_creationParams(params)
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