#include "PhysicsComponentSerializer.h"

#include <utils/JsonHelpers.h>
#include <components/PhysicsComponent.h>

namespace df3d { namespace components { namespace serializers {

shared_ptr<NodeComponent> PhysicsComponentSerializer::fromJson(const Json::Value &root)
{
    auto type = root["type"].asString();
    if (type != "StaticBody")
    {
        base::glog << "Unsupported rigid body type" << type << base::logwarn;
        return nullptr;
    }

    PhysicsComponent::CollisionShapeType colShapeType;
    PhysicsComponent::CreationParams params;

    auto shape = root["shape"].asString();
    if (shape == "box")
        colShapeType = PhysicsComponent::CollisionShapeType::BOX;
    else if (shape == "sphere")
        colShapeType = PhysicsComponent::CollisionShapeType::SPHERE;
    else if (shape == "convex_hull")
        colShapeType = PhysicsComponent::CollisionShapeType::CONVEX_HULL;
    else
    {
        base::glog << "Unsupported rigid body box shape" << shape << base::logwarn;
        return nullptr;
    }

    params.type = colShapeType;
    params.mass = utils::jsonGetValueWithDefault(root["mass"], params.mass);
    params.friction = utils::jsonGetValueWithDefault(root["friction"], params.friction);
    params.restitution = utils::jsonGetValueWithDefault(root["restitution"], params.restitution);
    params.linearDamping = utils::jsonGetValueWithDefault(root["linearDamping"], params.linearDamping);
    params.angularDamping = utils::jsonGetValueWithDefault(root["angularDamping"], params.angularDamping);
    params.group = utils::jsonGetValueWithDefault(root["group"], params.group);
    params.mask = utils::jsonGetValueWithDefault(root["mask"], params.mask);
    
    return make_shared<PhysicsComponent>(params);
}

Json::Value PhysicsComponentSerializer::toJson(shared_ptr<const NodeComponent> component)
{
    // TODO:
    assert(false);

    Json::Value result;
    
    return result;
}

} } }