#pragma once

#include <scene/Entity.h>
#include <math/AABB.h>
#include <math/BoundingSphere.h>
#include <math/OBB.h>

namespace df3d {

class MeshData;

class DF3D_DLL StaticMeshComponentProcessor : utils::NonCopyable
{
    // TODO: ecs: more cache friendly layout.
    struct ComponentData
    {
        shared_ptr<MeshData> m_meshData;

        // Transformed AABB.
        AABB m_transformedAABB;
        bool m_transformedAabbDirty = true;

        // Bounding sphere.
        BoundingSphere m_sphere;
        bool m_boundingSphereDirty = true;

        // Oriented bb.
        OBB m_obb;
        bool m_obbTransformationDirty = true;

        bool m_visible = true;
        bool m_frustumCullingDisabled = false;
    };

    std::vector<ComponentData> m_components;

public:
    StaticMeshComponentProcessor();
    ~StaticMeshComponentProcessor();

    shared_ptr<MeshData> getMeshData(ComponentInstance comp) const;

    AABB getAABB(ComponentInstance comp);
    BoundingSphere getBoundingSphere(ComponentInstance comp);
    OBB getOBB(ComponentInstance comp);

    void setVisible(ComponentInstance comp, bool visible);
    void disableFrustumCulling(ComponentInstance comp, bool disable);
};

}
