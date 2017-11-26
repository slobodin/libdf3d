#pragma once

namespace df3d {

enum class CollisionShapeType
{
    BOX,                            // btBoxShape
    SPHERE,                         // btSphereShape
    CONVEX_HULL,                    // btConvexHullShape
    STATIC_MESH,                    // btBvhTriangleMeshShape
    DYNAMIC_MESH,                   // btGimpactMeshShape

    UNDEFINED
};

struct PhysicsComponentCreationParams
{
    Id groupId;
    CollisionShapeType shape = CollisionShapeType::UNDEFINED;
    float mass = 1.0f;
    float friction = 0.5f;
    float restitution = 0.0f;
    float linearDamping = 0.0f;
    float angularDamping = 0.0f;
    bool disableDeactivation = false;
    bool noContactResponse = false;

    PhysicsComponentCreationParams();
    ~PhysicsComponentCreationParams();
    explicit PhysicsComponentCreationParams(const char *physicsDefinitionFile);
    explicit PhysicsComponentCreationParams(const Json::Value &root);

    Json::Value toJson() const;
};

}
