#pragma once

namespace df3d { namespace scene {

class BoundingSphere;

class DF3D_DLL Frustum
{
    enum
    {
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,
        FAR,
        NEAR
    };

    glm::vec4 m_planes[6];

public:
    Frustum();
    Frustum(glm::mat4 vp);
    ~Frustum();

    bool sphereInFrustum(const BoundingSphere &sphere) const;
};

} }
