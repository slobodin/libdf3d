#include "AABB.h"

#include <scene/WorldSize.h>

namespace df3d { namespace scene {

AABB::AABB()
{
    reset();
}

AABB::~AABB()
{
}

void AABB::reset()
{
    m_min.x = m_min.y = m_min.z = MAX_COORD_VALUE;
    m_max.x = m_max.y = m_max.z = -MAX_COORD_VALUE;
}

void AABB::updateBounds(const glm::vec3 &point)
{
    if (point.x < m_min.x)
        m_min.x = point.x;

    if (point.x > m_max.x)
        m_max.x = point.x;

    if (point.y < m_min.y)
        m_min.y = point.y;

    if (point.y > m_max.y)
        m_max.y = point.y;

    if (point.z < m_min.z)
        m_min.z = point.z;

    if (point.z > m_max.z)
        m_max.z = point.z;
}

const glm::vec3 &AABB::minPoint() const
{
    return m_min;
}

const glm::vec3 &AABB::maxPoint() const
{
    return m_max;
}

bool AABB::isValid() const
{
    return m_min.x < m_max.x && m_min.y < m_max.y && m_min.z < m_max.z;
}

bool AABB::contains(const glm::vec3 &point) const
{
    // TODO:
    assert(false);
    return true;
}

bool AABB::intersects(const AABB &other) const
{
    if (!isValid() || !other.isValid())
        return false;

    return ((m_min.x >= other.m_min.x && m_min.x <= other.m_max.x) || (other.m_min.x >= m_min.x && other.m_min.x <= m_max.x)) &&
        ((m_min.y >= other.m_min.y && m_min.y <= other.m_max.y) || (other.m_min.y >= m_min.y && other.m_min.y <= m_max.y)) &&
        ((m_min.z >= other.m_min.z && m_min.z <= other.m_max.z) || (other.m_min.z >= m_min.z && other.m_min.z <= m_max.z));
}

bool AABB::intersects(const BoundingSphere &sphere) const
{
    // TODO:
    assert(false);
    return false;
}

glm::vec3 AABB::getCenter() const
{
    return (m_min + m_max) / 2.f;
}

void AABB::getCorners(std::vector<glm::vec3> &output) const
{
    if (output.size() != 8)
    {
        base::glog << "Can not get corners of an AABB. Invalid input." << base::logwarn;
        return;
    }

    output[0] = glm::vec3(m_min.x, m_max.y, m_max.z);
    output[1] = glm::vec3(m_min.x, m_min.y, m_max.z);
    output[2] = glm::vec3(m_max.x, m_min.y, m_max.z);
    output[3] = glm::vec3(m_max.x, m_max.y, m_max.z);
    output[4] = glm::vec3(m_max.x, m_max.y, m_min.z);
    output[5] = glm::vec3(m_max.x, m_min.y, m_min.z);
    output[6] = glm::vec3(m_min.x, m_min.y, m_min.z);
    output[7] = glm::vec3(m_min.x, m_max.y, m_min.z);
}

} }
