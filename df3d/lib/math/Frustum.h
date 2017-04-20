#pragma once

#include <glm/glm.hpp>

namespace df3d {

class BoundingSphere;

class Frustum
{
    enum
    {
        PLANE_LEFT,
        PLANE_RIGHT,
        PLANE_TOP,
        PLANE_BOTTOM,
        PLANE_FAR,
        PLANE_NEAR
    };

    glm::vec4 m_planes[6];

public:
    Frustum();
    Frustum(glm::mat4 vp);
    ~Frustum();

    bool sphereInFrustum(const BoundingSphere &sphere) const;
};

}
