#include "PhysicsComponentCreationParams.h"

#include <df3d/lib/JsonUtils.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/io/FileSystem.h>

namespace df3d {

PhysicsComponentCreationParams::PhysicsComponentCreationParams()
{

}

PhysicsComponentCreationParams::~PhysicsComponentCreationParams()
{

}

PhysicsComponentCreationParams::PhysicsComponentCreationParams(const char *physicsDefinitionFile)
    : PhysicsComponentCreationParams(JsonUtils::fromFile(physicsDefinitionFile, svc().fileSystem()))
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
    else if (shapeStr == "static_triangle_mesh")
        shape = CollisionShapeType::STATIC_TRIANGLE_MESH;
    else if (shapeStr == "convex_decomposition")
        shape = CollisionShapeType::CONVEX_DECOMPOSITION;
    else
        DFLOG_WARN("Unsupported rigid body shape %s", shapeStr.c_str());

    root["mass"] >> mass;
    root["friction"] >> friction;
    root["restitution"] >> restitution;
    root["linearDamping"] >> linearDamping;
    root["angularDamping"] >> angularDamping;
    root["group"] >> group;
    root["mask"] >> mask;
    root["disableDeactivation"] >> disableDeactivation;
    root["noContactResponse"] >> noContactResponse;
}

}
