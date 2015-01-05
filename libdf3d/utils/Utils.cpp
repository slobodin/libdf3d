#include "df3d_pch.h"
#include "Utils.h"

namespace df3d { namespace utils { namespace math {

const glm::vec4 XAxis = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
const glm::vec4 YAxis = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
const glm::vec4 ZAxis = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

glm::quat fromToRotation(const glm::vec3 &v1, const glm::vec3 &v2)
{
    glm::quat q;
    const auto a = glm::cross(v1, v2);
    const auto v1lenSq = glm::length2(v1);
    const auto v2lenSq = glm::length2(v2);

    q.x = a.x;
    q.y = a.y;
    q.z = a.z;
    q.w = sqrt(v1lenSq * v2lenSq) + glm::dot(v1, v2);

    return glm::normalize(q);
}

glm::vec2 toPolar(const glm::vec2 &cartesian)
{
    return { std::hypot(cartesian.x, cartesian.y), std::atan2(cartesian.y, cartesian.x) };
}

} } }