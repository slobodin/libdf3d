#pragma once

#include "BoundingVolume.h"

namespace df3d { namespace scene {

class AABB;

class DF3D_DLL BoundingSphere : public BoundingVolume
{
    glm::vec3 m_position;
    float m_radius;

public:
    BoundingSphere();
    ~BoundingSphere();

    void reset();
    void updateBounds(const glm::vec3 &point);
    void setPosition(const glm::vec3 &pos);
    void setRadius(float radius);

    bool isValid() const;

    bool contains(const glm::vec3 &point) const;
    bool intersects(const BoundingSphere &other) const;
    bool intersects(const AABB &aabb) const;

    const glm::vec3 &getCenter() const;
    const float getRadius() const;
};

} }