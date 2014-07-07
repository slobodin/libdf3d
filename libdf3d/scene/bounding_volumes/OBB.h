#pragma once

#include "BoundingVolume.h"

namespace df3d { namespace scene {

class DF3D_DLL OBB : public BoundingVolume
{
    glm::vec3 m_min;
    glm::vec3 m_max;

    glm::mat4 m_transformation;

public:
    OBB();
    ~OBB();

    void reset();
    void updateBounds(const glm::vec3 &point);

    const glm::vec3 &minPoint() const;
    const glm::vec3 &maxPoint() const;

    bool isValid() const;

    bool intersects(const OBB &other) const;

    void setTransformation(const glm::mat4 &transformation);
    const glm::mat4 &getTransformation() const;
};

} }