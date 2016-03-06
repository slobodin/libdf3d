#include "PhysicsComponentCreationParams.h"

#include <libdf3d/utils/JsonUtils.h>

namespace df3d {

PhysicsComponentCreationParams::PhysicsComponentCreationParams()
{

}

PhysicsComponentCreationParams::~PhysicsComponentCreationParams()
{

}

PhysicsComponentCreationParams::PhysicsComponentCreationParams(const std::string &physicsDefinitionFile)
    : PhysicsComponentCreationParams(utils::json::fromFile(physicsDefinitionFile))
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
    else
        glog << "Unsupported rigid body shape" << shapeStr << logwarn;

    root["mass"] >> mass;
    root["friction"] >> friction;
    root["restitution"] >> restitution;
    root["linearDamping"] >> linearDamping;
    root["angularDamping"] >> angularDamping;
    root["group"] >> group;
    root["mask"] >> mask;
    root["disableDeactivation"] >> disableDeactivation;
}

}
