#pragma once

#include "BoundingVolume.h"

namespace df3d { namespace scene {

class BoundingSphere;

class DF3D_DLL AABB : public BoundingVolume
{
    glm::vec3 m_min;
    glm::vec3 m_max;
public:
    AABB();
    ~AABB();

    void reset();
    void updateBounds(const glm::vec3 &point);

    const glm::vec3 &minPoint() const;
    const glm::vec3 &maxPoint() const;

    bool isValid() const;

    bool contains(const glm::vec3 &point) const;
    bool intersects(const AABB &other) const;
    bool intersects(const BoundingSphere &sphere) const;

    glm::vec3 getCenter() const;
    // Must be size of 8.
    void getCorners(std::vector<glm::vec3> &output);
};

} }