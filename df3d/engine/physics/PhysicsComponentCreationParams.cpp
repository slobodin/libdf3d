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
    auto shapeStr = root["shape"].asString();
    if (shapeStr == "box")
        shape = CollisionShapeType::BOX;
    else if (shapeStr == "sphere")
        shape = CollisionShapeType::SPHERE;
    else if (shapeStr == "convex_hull")
        shape = CollisionShapeType::CONVEX_HULL;
    else if (shapeStr == "static_mesh")
        shape = CollisionShapeType::STATIC_MESH;
    else if (shapeStr == "convex_decomposition")
        shape = CollisionShapeType::CONVEX_DECOMPOSITION;
    else if (shapeStr == "dynamic_mesh")
        shape = CollisionShapeType::DYNAMIC_MESH;
    else
        DFLOG_WARN("Unsupported rigid body shape %s", shapeStr.c_str());

    root["mass"] >> mass;
    root["friction"] >> friction;
    root["restitution"] >> restitution;
    root["linearDamping"] >> linearDamping;
    root["angularDamping"] >> angularDamping;
    root["collisionGroup"] >> groupId;
    root["disableDeactivation"] >> disableDeactivation;
    root["noContactResponse"] >> noContactResponse;
}

}
