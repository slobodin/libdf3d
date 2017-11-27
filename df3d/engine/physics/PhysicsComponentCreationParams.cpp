#include "PhysicsComponentCreationParams.h"

#include <df3d/lib/JsonUtils.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceFileSystem.h>

namespace df3d {

PhysicsComponentCreationParams::PhysicsComponentCreationParams()
{

}

PhysicsComponentCreationParams::~PhysicsComponentCreationParams()
{

}

PhysicsComponentCreationParams::PhysicsComponentCreationParams(const char *physicsDefinitionFile)
    : PhysicsComponentCreationParams(JsonUtils::fromFile(physicsDefinitionFile))
{

}

void PhysicsComponentCreationParams::setShape(const std::string &shapeStr)
{
    if (shapeStr == "box")
        shape = CollisionShapeType::BOX;
    else if (shapeStr == "sphere")
        shape = CollisionShapeType::SPHERE;
    else if (shapeStr == "convex_hull")
        shape = CollisionShapeType::CONVEX_HULL;
    else if (shapeStr == "static_mesh")
        shape = CollisionShapeType::STATIC_MESH;
    else if (shapeStr == "dynamic_mesh")
        shape = CollisionShapeType::DYNAMIC_MESH;
    else
        DFLOG_WARN("Unsupported rigid body shape %s", shapeStr.c_str());
}

PhysicsComponentCreationParams::PhysicsComponentCreationParams(const Json::Value &root)
{
    DF3D_ASSERT(root.isMember("shape"));
    setShape(root["shape"].asString());

    mass = JsonUtils::get(root, "mass", mass);
    friction = JsonUtils::get(root, "friction", friction);
    restitution = JsonUtils::get(root, "restitution", restitution);
    linearDamping = JsonUtils::get(root, "linearDamping", linearDamping);
    angularDamping = JsonUtils::get(root, "angularDamping", angularDamping);
    disableDeactivation = JsonUtils::get(root, "disableDeactivation", disableDeactivation);
    noContactResponse = JsonUtils::get(root, "noContactResponse", noContactResponse);
    groupId = Id(root["collisionGroup"].asCString());
}

Json::Value PhysicsComponentCreationParams::toJson() const
{
    Json::Value result;

    result["mass"] = mass;
    result["friction"] = friction;
    result["restitution"] = restitution;
    result["linearDamping"] = linearDamping;
    result["angularDamping"] = angularDamping;
    result["disableDeactivation"] = disableDeactivation;
    result["noContactResponse"] = noContactResponse;
    result["collisionGroup"] = groupId.toString();

    switch (shape)
    {
    case CollisionShapeType::BOX:
        result["shape"] = "box";
        break;
    case CollisionShapeType::SPHERE:
        result["shape"] = "sphere";
        break;
    case CollisionShapeType::CONVEX_HULL:
        result["shape"] = "convex_hull";
        break;
    case CollisionShapeType::STATIC_MESH:
        result["shape"] = "static_mesh";
        break;
    case CollisionShapeType::DYNAMIC_MESH:
        result["shape"] = "dynamic_mesh";
        break;
    default:
        DF3D_ASSERT(false);
        break;
    }

    return result;
}

}
