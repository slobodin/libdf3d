#include "OBB.h"

#include <scene/WorldSize.h>

namespace df3d { namespace scene {

OBB::OBB()
{
    reset();
}

OBB::~OBB()
{
}

void OBB::reset()
{
    m_min.x = m_min.y = m_min.z = MAX_COORD_VALUE;
    m_max.x = m_max.y = m_max.z = -MAX_COORD_VALUE;
}

void OBB::updateBounds(const glm::vec3 &point)
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

const glm::vec3 &OBB::minPoint() const
{
    return m_min;
}

const glm::vec3 &OBB::maxPoint() const
{
    return m_max;
}

bool OBB::isValid() const
{
    return m_min.x < m_max.x && m_min.y < m_max.y && m_min.z < m_max.z;
}

bool OBB::intersects(const OBB &other) const
{
    if (!isValid() || !other.isValid())
        return false;

    return false;
}

void OBB::setTransformation(const glm::mat4 &transformation)
{
    m_transformation = transformation;
}

const glm::mat4 &OBB::getTransformation() const
{
    return m_transformation;
}

} }
