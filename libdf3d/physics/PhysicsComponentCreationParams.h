#pragma once

namespace df3d {

enum class CollisionShapeType
{
    BOX,
    SPHERE,
    CONVEX_HULL
};

struct DF3D_DLL PhysicsComponentCreationParams
{
    CollisionShapeType type = CollisionShapeType::BOX;

    float mass = 1.0f;
    float friction = 0.5f;
    float restitution = 0.0f;
    float linearDamping = 0.0f;
    float angularDamping = 0.0f;
    short group = -1;
    short mask = -1;

    PhysicsComponentCreationParams() = default;
    explicit PhysicsComponentCreationParams(const std::string &physicsDefinitionFile);
    explicit PhysicsComponentCreationParams(const Json::Value &root);
};

}
