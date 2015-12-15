#include "df3d_pch.h"
#include "PhysicsComponentProcessor.h"

#include <math/AABB.h>
#include <math/BoundingSphere.h>
#include <math/ConvexHull.h>

namespace df3d {

PhysicsComponentProcessor::PhysicsComponentProcessor()
{

}

PhysicsComponentProcessor::~PhysicsComponentProcessor()
{

}

btRigidBody* PhysicsComponentProcessor::body(ComponentInstance comp)
{
    return nullptr;
}

ComponentInstance PhysicsComponentProcessor::add(Entity e, const CreationParams &params, const AABB &box)
{
    return ComponentInstance();
}

ComponentInstance PhysicsComponentProcessor::add(Entity e, const CreationParams &params, const BoundingSphere &sphere)
{
    return ComponentInstance();
}

ComponentInstance PhysicsComponentProcessor::add(Entity e, const CreationParams &params, const ConvexHull &hull)
{
    return ComponentInstance();
}

void PhysicsComponentProcessor::remove(Entity e)
{

}

ComponentInstance PhysicsComponentProcessor::lookup(Entity e)
{
    return ComponentInstance();
}

}
