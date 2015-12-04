#pragma once

#include "NodeComponent.h"
#include <math/AABB.h>
#include <math/BoundingSphere.h>
#include <math/OBB.h>

namespace df3d {

class MeshData;
class Material;

// TODO: this is static mesh, create animated mesh.

class DF3D_DLL MeshComponent : public NodeComponent
{
    bool isInFov();

    void onComponentEvent(ComponentEvent ev) override;
    void onDraw(RenderQueue *ops) override;
    void onUpdate(float dt) override;

    shared_ptr<MeshData> m_meshData;

    // Transformed AABB.
    AABB m_transformedAABB;
    bool m_transformedAabbDirty = true;
    void constructTransformedAABB();

    // Bounding sphere.
    BoundingSphere m_sphere;
    bool m_boundingSphereDirty = true;
    void constructBoundingSphere();
    void updateBoundingSpherePosition();

    // Oriented bb.
    OBB m_obb;
    bool m_obbTransformationDirty = true;
    void updateOBB();

    bool m_visible = true;
    bool m_frustumCullingDisabled = false;

    struct ResourceMgrListenerImpl;
    unique_ptr<ResourceMgrListenerImpl> m_rmgrListener;
    bool m_meshWasLoaded = false;   // little workaround

    void setMeshData(shared_ptr<MeshData> meshData);

protected:
    MeshComponent();

public:
    MeshComponent(const std::string &meshFilePath, ResourceLoadingMode lm = ResourceLoadingMode::ASYNC);
    MeshComponent(shared_ptr<MeshData> meshData);
    ~MeshComponent();

    shared_ptr<MeshData> getMeshData() const;

    AABB getAABB();
    BoundingSphere getBoundingSphere();
    OBB getOBB();

    void setVisible(bool visible) { m_visible = visible; }
    void disableFrustumCulling(bool disable) { m_frustumCullingDisabled = disable; }

    shared_ptr<NodeComponent> clone() const override;
};

}
