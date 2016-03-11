#include "BoundingSphere.h"

#include <libdf3d/utils/MathUtils.h>
#include <glm/gtx/intersect.hpp>

namespace df3d {

BoundingSphere::BoundingSphere()
{
    reset();
}

BoundingSphere::~BoundingSphere()
{
}

void BoundingSphere::reset()
{
    m_radius = -1.0f;
    m_position = glm::vec3(0.0f, 0.0f, 0.0f);
}

void BoundingSphere::updateBounds(const glm::vec3 &point)
{
    auto len = glm::length(point - m_position);
    if (len > m_radius)
        m_radius = len;
}

void BoundingSphere::setPosition(const glm::vec3 &pos)
{
    m_position = pos;
}

void BoundingSphere::setRadius(float radius)
{
    m_radius = radius;
}

bool BoundingSphere::isValid() const
{
    return m_radius > 0.0f;
}

bool BoundingSphere::contains(const glm::vec3 &point) const
{
    if (!isValid())
        return false;
    return glm::length(point - m_position) <= m_radius;
}

bool BoundingSphere::intersects(const BoundingSphere &other) const
{
    if (!isValid())
        return false;
    return glm::length(other.m_position - m_position) <= m_radius + other.m_radius;
}

bool BoundingSphere::intersects(const AABB &aabb) const
{
    if (!isValid())
        return false;
    // TODO:
    DF3D_ASSERT(false, "not implemented");
    return false;
}

bool BoundingSphere::intersects(const utils::math::Ray &r, glm::vec3 &outPos) const
{
    if (!isValid())
        return false;
    glm::vec3 normal;
    return glm::intersectRaySphere(r.origin, r.dir, m_position, m_radius, outPos, normal);
}

const glm::vec3 &BoundingSphere::getCenter() const
{
    return m_position;
}

const float BoundingSphere::getRadius() const
{
    return m_radius;
}

}
