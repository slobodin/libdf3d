#pragma once

namespace df3d {

enum class CollisionShapeType
{
    BOX,                            // btBoxShape
    SPHERE,                         // btSphereShape
    CONVEX_HULL,                    // btConvexHullShape
    STATIC_TRIANGLE_MESH,           // btBvhTriangleMeshShape

    UNDEFINED
};

struct DF3D_DLL PhysicsComponentCreationParams
{
    CollisionShapeType shape = CollisionShapeType::UNDEFINED;

    float mass = 1.0f;
    float friction = 0.5f;
    float restitution = 0.0f;
    float linearDamping = 0.0f;
    float angularDamping = 0.0f;
    short group = -1;
    short mask = -1;
    bool disableDeactivation = false;
    bool noContactResponse = false;

    PhysicsComponentCreationParams();
    ~PhysicsComponentCreationParams();
    explicit PhysicsComponentCreationParams(const std::string &physicsDefinitionFile);
    explicit PhysicsComponentCreationParams(const Json::Value &root);
};

}
