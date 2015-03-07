#include "df3d_pch.h"
#include "Frustum.h"

#include "bounding_volumes/BoundingSphere.h"
#include <utils/Utils.h>

namespace df3d { namespace scene {

void Frustum::normalizePlane(glm::vec4 &plane)
{
    auto len = glm::length(glm::vec3(plane));

    plane.x /= len;
    plane.y /= len;
    plane.z /= len;
    plane.w /= len;
}

Frustum::Frustum()
{

}

Frustum::Frustum(glm::mat4 vp)
{
    // Extract planes from viewprojection matrix. Result vec4(A, B, C, D)
    m_planes[LEFT] = glm::column(vp, 0) + glm::column(vp, 3);
    m_planes[RIGHT] = -glm::column(vp, 0) + glm::column(vp, 3);

    m_planes[TOP] = -glm::column(vp, 1) + glm::column(vp, 3);
    m_planes[BOTTOM] = glm::column(vp, 1) + glm::column(vp, 3);
    
    m_planes[FAR] = -glm::column(vp, 2) + glm::column(vp, 3);
    m_planes[NEAR] = glm::column(vp, 2) + glm::column(vp, 3);

    for (int i = 0; i < 6; i++)
        normalizePlane(m_planes[i]);
}

Frustum::~Frustum()
{

}

bool Frustum::sphereInFrustum(const BoundingSphere &sphere) const
{
    const auto &center = sphere.getCenter();
    auto radius = sphere.getRadius();

    for (int i = 0; i < 6; i++) 
    {
        auto dist = utils::math::distanceToPlane(m_planes[i], sphere.getCenter());
        if (dist < -radius)
            return true;
    }

    return true;
}

} }
