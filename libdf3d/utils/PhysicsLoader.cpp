#include "df3d_pch.h"
#include "PhysicsLoader.h"

#include <utils/JsonHelpers.h>
#include <components/PhysicsComponent.h>
#include <components/MeshComponent.h>
#include <components/TransformComponent.h>
#include <base/Controller.h>
#include <physics/PhysicsManager.h>
#include <render/SubMesh.h>
#include <render/VertexIndexBuffer.h>
#include <render/MeshData.h>
#include <scene/Node.h>

namespace df3d { namespace utils { namespace physics_loader {

void init(components::PhysicsComponent *component, const char *definitionFile)
{
    auto root = utils::jsonLoadFromFile(definitionFile);
    if (root.empty())
        return;

    auto type = root["type"].asString();
    if (type != "StaticBody")
    {
        base::glog << "Unsupported rigid body type" << type << base::logwarn;
        return;
    }

    auto mesh = component->getHolder()->mesh();
    if (!mesh || !mesh->isGeometryValid())
    {
        base::glog << "Can not create rigid body. Node doesn't have mesh component or it's invalid." << base::logwarn;
        return;
    }

    btCollisionShape *colShape = nullptr;
    auto shape = root["shape"].asString();
    if (shape == "box")
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
    else if (shape == "sphere")
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
    else if (shape == "convex_hull")
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

        colShape->setLocalScaling(physics::glmTobt(component->getHolder()->transform()->getScale()));
    }

    if (!colShape)
    {
        base::glog << "Unsupported rigid body box shape" << shape << base::logwarn;
        return;
    }

    float mass = utils::jsonGetValueWithDefault(root["mass"], 1.0f);
    float friction = utils::jsonGetValueWithDefault(root["friction"], 0.5f);
    float restitution = utils::jsonGetValueWithDefault(root["restitution"], 0.0f);
    float linearDamping = utils::jsonGetValueWithDefault(root["linearDamping"], 0.0f);
    float angularDamping = utils::jsonGetValueWithDefault(root["angularDamping"], 0.0f);

    // Set initial transformation.
    btTransform startTransform = btTransform::getIdentity();

    auto pos = component->getHolder()->transform()->getPosition();
    auto orient = component->getHolder()->transform()->getRotation(true);
    //auto orient = node->getOrientation();

    startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
    startTransform.setRotation(btQuaternion(orient.y, orient.x, orient.z));
    //startTransform.setRotation(btQuaternion(orient.x, orient.y, orient.z, orient.w));

    btVector3 localInertia(0, 0, 0);
    if (!glm::epsilonEqual(mass, 0.0f, glm::epsilon<float>()))
        colShape->calculateLocalInertia(mass, localInertia);

    // Set motion state.
    auto myMotionState = new physics::NodeMotionState(startTransform, component->getHolder());

    // Fill body properties.
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
    rbInfo.m_friction = friction;
    rbInfo.m_restitution = restitution;
    rbInfo.m_linearDamping = linearDamping;
    rbInfo.m_angularDamping = angularDamping;

    component->body = new btRigidBody(rbInfo);
}

} } }