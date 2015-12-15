#pragma once

#include <scene/Entity.h>

class btRigidBody;

namespace df3d {

class AABB;
class BoundingSphere;
class ConvexHull;

class DF3D_DLL PhysicsComponentProcessor : utils::NonCopyable
{
public:
    struct CreationParams
    {
        float mass = 1.0f;
        float friction = 0.5f;
        float restitution = 0.0f;
        float linearDamping = 0.0f;
        float angularDamping = 0.0f;
        short group = -1;
        short mask = -1;
    };

private:

public:
    PhysicsComponentProcessor();
    ~PhysicsComponentProcessor();

    btRigidBody* body(ComponentInstance comp);

    ComponentInstance add(Entity e, const CreationParams &params, const AABB &box);
    ComponentInstance add(Entity e, const CreationParams &params, const BoundingSphere &sphere);
    ComponentInstance add(Entity e, const CreationParams &params, const ConvexHull &hull);
    void remove(Entity e);
    ComponentInstance lookup(Entity e);
};

}
