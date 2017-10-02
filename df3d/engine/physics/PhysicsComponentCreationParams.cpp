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

PhysicsComponentCreationParams::PhysicsComponentCreationParams(const Json::Value &root)
{
    DF3D_ASSERT(root.isMember("shape"));

    auto shapeStr = Id(root["shape"].asCString());
    if (shapeStr == Id("box"))
        shape = CollisionShapeType::BOX;
    else if (shapeStr == Id("sphere"))
        shape = CollisionShapeType::SPHERE;
    else if (shapeStr == Id("convex_hull"))
        shape = CollisionShapeType::CONVEX_HULL;
    else if (shapeStr == Id("static_mesh"))
        shape = CollisionShapeType::STATIC_MESH;
    else if (shapeStr == Id("dynamic_mesh"))
        shape = CollisionShapeType::DYNAMIC_MESH;
    else
        DFLOG_WARN("Unsupported rigid body shape %s", root["shape"].asCString());

    mass = JsonUtils::get(root, "mass", mass);
    friction = JsonUtils::get(root, "friction", friction);
    restitution = JsonUtils::get(root, "restitution", restitution);
    linearDamping = JsonUtils::get(root, "linearDamping", linearDamping);
    angularDamping = JsonUtils::get(root, "angularDamping", angularDamping);
    disableDeactivation = JsonUtils::get(root, "disableDeactivation", disableDeactivation);
    noContactResponse = JsonUtils::get(root, "noContactResponse", noContactResponse);
    groupId = Id(root["collisionGroup"].asCString());
}

}
