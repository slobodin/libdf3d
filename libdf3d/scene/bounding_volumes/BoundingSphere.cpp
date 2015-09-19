#include "BoundingSphere.h"

namespace df3d { namespace scene {

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
    return glm::length(point - m_position) <= m_radius;
}

bool BoundingSphere::intersects(const BoundingSphere &other) const
{
    return glm::length(other.m_position - m_position) <= m_radius + other.m_radius;
}

bool BoundingSphere::intersects(const AABB &aabb) const
{
    // TODO:
    assert(false);
    return false;
}

const glm::vec3 &BoundingSphere::getCenter() const
{
    return m_position;
}

const float BoundingSphere::getRadius() const
{
    return m_radius;
}

} }
