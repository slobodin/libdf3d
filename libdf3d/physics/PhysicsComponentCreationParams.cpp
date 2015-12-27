#include "PhysicsComponentCreationParams.h"

#include <utils/JsonUtils.h>

namespace df3d {

PhysicsComponentCreationParams::PhysicsComponentCreationParams(const std::string &physicsDefinitionFile)
    : PhysicsComponentCreationParams(utils::json::fromFile(physicsDefinitionFile))
{

}

PhysicsComponentCreationParams::PhysicsComponentCreationParams(const Json::Value &root)
{
    shape = CollisionShapeType::BOX;

    auto shapeStr = root["shape"].asString();
    if (shapeStr == "box")
        shape = CollisionShapeType::BOX;
    else if (shapeStr == "sphere")
        shape = CollisionShapeType::SPHERE;
    else if (shapeStr == "convex_hull")
        shape = CollisionShapeType::CONVEX_HULL;
    else
        glog << "Unsupported rigid body box shape" << shapeStr << logwarn;

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
