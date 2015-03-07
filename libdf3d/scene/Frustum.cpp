#include "df3d_pch.h"
#include "Frustum.h"

#include "bounding_volumes/BoundingSphere.h"
#include <utils/Utils.h>

namespace df3d { namespace scene {

Frustum::Frustum()
{

}

Frustum::Frustum(glm::mat4 vp)
{
    // Extract planes from viewprojection matrix. Result vec4(A, B, C, D)
    m_planes[LEFT] = glm::row(vp, 0) + glm::row(vp, 3);
    m_planes[RIGHT] = -glm::row(vp, 0) + glm::row(vp, 3);

    m_planes[TOP] = -glm::row(vp, 1) + glm::row(vp, 3);
    m_planes[BOTTOM] = glm::row(vp, 1) + glm::row(vp, 3);

    m_planes[FAR] = -glm::row(vp, 2) + glm::row(vp, 3);
    m_planes[NEAR] = glm::row(vp, 2) + glm::row(vp, 3);

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
