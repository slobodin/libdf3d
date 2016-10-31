#pragma once

#include <df3d/lib/Handles.h>
#include <df3d/game/Entity.h>
#include <LinearMath/btVector3.h>
#include <LinearMath/btQuaternion.h>
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>

namespace df3d {

class PhysicsHelpers
{
public:
    static btVector3 glmTobt(const glm::vec3 &v)
    {
        return btVector3(v.x, v.y, v.z);
    }

    static btQuaternion glmTobt(const glm::quat &q)
    {
        return btQuaternion(q.x, q.y, q.z, q.w);
    }

    static glm::vec3 btToGlm(const btVector3 &v)
    {
        return glm::vec3(v.x(), v.y(), v.z());
    }

    static glm::quat btToGlm(const btQuaternion &q)
    {
        return glm::quat(q.w(), q.x(), q.y(), q.z());
    }

    static df3d::Entity getEntity(const btCollisionObject *obj)
    {
        if (!obj)
            return{};

        auto userIdx = obj->getUserIndex();
        return *reinterpret_cast<Entity*>(&userIdx);
    }
};

}
