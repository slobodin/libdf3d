#include "MathUtils.h"

#include <glm/gtx/fast_square_root.hpp>

namespace df3d {

const glm::vec4 MathUtils::XAxis = { 1.0f, 0.0f, 0.0f, 0.0f };
const glm::vec4 MathUtils::YAxis = { 0.0f, 1.0f, 0.0f, 0.0f };
const glm::vec4 MathUtils::ZAxis = { 0.0f, 0.0f, 1.0f, 0.0f };
const glm::vec3 MathUtils::UnitVec3 = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));

glm::quat MathUtils::fromToRotation(const glm::vec3 &v1, const glm::vec3 &v2)
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

glm::vec2 MathUtils::toPolar(const glm::vec2 &cartesian)
{
    return { std::sqrt(cartesian.x * cartesian.x + cartesian.y * cartesian.y), std::atan2(cartesian.y, cartesian.x) };
}

glm::vec2 MathUtils::fromPolar(float angle, float dist)
{
    return dist * glm::vec2(std::cos(angle), std::sin(angle));
}

float MathUtils::signedDistanceToPlane(const glm::vec4 &plane, const glm::vec3 &point)
{
    return glm::dot(glm::vec3(plane), point) + plane.w;
}

glm::vec3 MathUtils::safeNormalize(const glm::vec3 &v)
{
    glm::vec3 res = v;
    if (glm::fastLength(res) > 0.00001f)
        res = glm::normalize(res);
    return res;
}

float MathUtils::gaussian(float x, float mean, float stddev)
{
    auto a = 1.0f / (stddev * std::sqrt(glm::two_pi<float>()));

    return a * std::exp(-((x - mean) * (x - mean) / 2.0f / stddev / stddev));
}

Spherical::Spherical(float r, float yaw, float pitch)
    : r(r), yaw(yaw), pitch(pitch)
{

}

Spherical::Spherical(const glm::vec3 &v)
{
    r = glm::length(v);

    if (r > 0.0f)
    {
        pitch = glm::asin(-v.y / r);
        if (glm::abs(pitch) >= glm::half_pi<float>() * 0.9999f)
            yaw = 0.0f;
        else
            yaw = std::atan2(v.x, v.z);
    }
    else
    {
        yaw = pitch = 0.0f;
    }
}

Spherical::~Spherical()
{

}

glm::vec3 Spherical::toCartesian()
{
    float x = r * glm::sin(pitch) * glm::cos(yaw);
    float y = r * glm::sin(pitch) * glm::sin(yaw);
    float z = r * glm::cos(pitch);

    return { x, y, z };
}

}
