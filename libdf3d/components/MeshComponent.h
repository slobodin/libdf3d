#pragma once

#include "NodeComponent.h"
#include <scene/bounding_volumes/AABB.h>
#include <scene/bounding_volumes/BoundingSphere.h>
#include <scene/bounding_volumes/OBB.h>

FWD_MODULE_CLASS(render, MeshData)
FWD_MODULE_CLASS(render, Material)

namespace df3d { namespace components {

class DF3D_DLL MeshComponent : public NodeComponent
{
    bool isInFov();

    void onEvent(components::ComponentEvent ev) override;
    void onDraw(render::RenderQueue *ops) override;

    shared_ptr<render::MeshData> m_geometry;

    // AABB in model space.
    scene::AABB m_aabb;
    // Transformed AABB.
    scene::AABB m_transformedAABB;
    bool m_aabbDirty = true;
    bool m_transformedAabbDirty = true;
    void constructAABB();
    void constructTransformedAABB();

    // Bounding sphere.
    scene::BoundingSphere m_sphere;
    bool m_boundingSphereDirty = true;
    void constructBoundingSphere();
    void updateBoundingSpherePosition();

    // Oriented bb.
    scene::OBB m_obb;
    bool m_obbDirty = true;
    bool m_obbTransformationDirty = true;
    void constructOBB();

    bool m_visible = true;
    bool m_frustumCullingDisabled = false;

protected:
    MeshComponent();

public:
    MeshComponent(const char *meshFilePath);
    ~MeshComponent();

    virtual void setGeometry(shared_ptr<render::MeshData> geometry);
    virtual shared_ptr<render::MeshData> getGeometry() { return m_geometry; }
    virtual bool isGeometryValid() const;

    virtual void setMaterial(shared_ptr<render::Material> material, size_t submeshIdx);
    virtual shared_ptr<render::Material> getMaterial(size_t submeshIdx);

    scene::AABB getAABB();
    scene::BoundingSphere getBoundingSphere();
    scene::OBB getOBB();

    std::string getMeshFilePath() const;
    void setVisible(bool visible) { m_visible = visible; }
    void disableFrustumCulling(bool disable) { m_frustumCullingDisabled = disable; }

    shared_ptr<NodeComponent> clone() const override;
};

} }