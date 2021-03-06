#pragma once

#include "BoundingVolume.h"

namespace df3d {

class AABB;
struct Ray;

class BoundingSphere : public BoundingVolume
{
    glm::vec3 m_position;
    float m_radius;

public:
    BoundingSphere();
    ~BoundingSphere();

    void reset() override;
    void updateBounds(const glm::vec3 &point) override;
    void setPosition(const glm::vec3 &pos);
    void setRadius(float radius);

    bool isValid() const override;

    bool contains(const glm::vec3 &point) const;
    bool intersects(const BoundingSphere &other) const;
    bool intersects(const AABB &aabb) const;
    bool intersects(const Ray &r, glm::vec3 &outPos) const;

    const glm::vec3 &getCenter() const;
    const float getRadius() const;
};

}
