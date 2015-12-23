#include "PhysicsComponentCreationParams.h"

#include <utils/JsonUtils.h>

namespace df3d {

PhysicsComponentCreationParams::PhysicsComponentCreationParams(const std::string &physicsDefinitionFile)
    : PhysicsComponentCreationParams(utils::json::fromFile(physicsDefinitionFile))
{

}

PhysicsComponentCreationParams::PhysicsComponentCreationParams(const Json::Value &root)
{
    type = CollisionShapeType::BOX;

    auto shape = root["shape"].asString();
    if (shape == "box")
        type = CollisionShapeType::BOX;
    else if (shape == "sphere")
        type = CollisionShapeType::SPHERE;
    else if (shape == "convex_hull")
        type = CollisionShapeType::CONVEX_HULL;
    else
        glog << "Unsupported rigid body box shape" << shape << logwarn;

    root["mass"] >> mass;
    root["friction"] >> friction;
    root["restitution"] >> restitution;
    root["linearDamping"] >> linearDamping;
    root["angularDamping"] >> angularDamping;
    root["group"] >> group;
    root["mask"] >> mask;
}

}
