#pragma once

#include <LinearMath/btVector3.h>
#include <LinearMath/btQuaternion.h>

namespace df3d {

inline btVector3 glmTobt(const glm::vec3 &v)
{
    return btVector3(v.x, v.y, v.z);
}

inline btQuaternion glmTobt(const glm::quat &q)
{
    return btQuaternion(q.x, q.y, q.z, q.w);
}

inline glm::vec3 btToGlm(const btVector3 &v)
{
    return glm::vec3(v.x(), v.y(), v.z());
}

}
