#include "Frustum.h"

#include "bounding_volumes/BoundingSphere.h"
#include <utils/MathUtils.h>

namespace df3d { namespace scene {

Frustum::Frustum()
{

}

Frustum::Frustum(glm::mat4 vp)
{
    // Extract planes from viewprojection matrix. Result vec4(A, B, C, D)
    m_planes[PLANE_LEFT] = glm::row(vp, 0) + glm::row(vp, 3);
    m_planes[PLANE_RIGHT] = -glm::row(vp, 0) + glm::row(vp, 3);

    m_planes[PLANE_TOP] = -glm::row(vp, 1) + glm::row(vp, 3);
    m_planes[PLANE_BOTTOM] = glm::row(vp, 1) + glm::row(vp, 3);

    m_planes[PLANE_FAR] = -glm::row(vp, 2) + glm::row(vp, 3);
    m_planes[PLANE_NEAR] = glm::row(vp, 2) + glm::row(vp, 3);

    for (int i = 0; i < 6; i++)
        m_planes[i] /= glm::length(glm::vec3(m_planes[i]));
}

Frustum::~Frustum()
{

}

bool Frustum::sphereInFrustum(const BoundingSphere &sphere) const
{
    auto center = sphere.getCenter();
    auto radius = sphere.getRadius();

    for (int i = 0; i < 6; i++)
    {
        auto dist = utils::math::signedDistanceToPlane(m_planes[i], center);
        if (dist < -radius)
            return false;
    }

    return true;
}

} }
