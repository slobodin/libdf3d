#pragma once

#include "NodeComponent.h"
#include <scene/bounding_volumes/AABB.h>
#include <scene/bounding_volumes/BoundingSphere.h>
#include <scene/bounding_volumes/OBB.h>

FWD_MODULE_CLASS(render, MeshData)
FWD_MODULE_CLASS(render, Material)

namespace df3d { namespace components {

// TODO: this is static mesh, create animated mesh.

class DF3D_DLL MeshComponent : public NodeComponent
{
    bool isInFov();

    void onComponentEvent(components::ComponentEvent ev) override;
    void onDraw(render::RenderQueue *ops) override;
    void onUpdate(float dt) override;

    shared_ptr<render::MeshData> m_meshData;

    // Transformed AABB.
    scene::AABB m_transformedAABB;
    bool m_transformedAabbDirty = true;
    void constructTransformedAABB();

    // Bounding sphere.
    scene::BoundingSphere m_sphere;
    bool m_boundingSphereDirty = true;
    void constructBoundingSphere();
    void updateBoundingSpherePosition();

    // Oriented bb.
    scene::OBB m_obb;
    bool m_obbTransformationDirty = true;
    void updateOBB();

    bool m_visible = true;
    bool m_frustumCullingDisabled = false;

    struct ResourceMgrListenerImpl;
    unique_ptr<ResourceMgrListenerImpl> m_rmgrListener;
    bool m_meshWasLoaded = false;   // little workaround

    void setMeshData(shared_ptr<render::MeshData> meshData);

protected:
    MeshComponent();

public:
    MeshComponent(const std::string &meshFilePath, ResourceLoadingMode lm = ResourceLoadingMode::ASYNC);
    MeshComponent(shared_ptr<render::MeshData> meshData);
    ~MeshComponent();

    shared_ptr<render::MeshData> getMeshData() const;

    scene::AABB getAABB();
    scene::BoundingSphere getBoundingSphere();
    scene::OBB getOBB();

    void setVisible(bool visible) { m_visible = visible; }
    void disableFrustumCulling(bool disable) { m_frustumCullingDisabled = disable; }

    shared_ptr<NodeComponent> clone() const override;
};

} }
