#pragma once

#include <scene/Entity.h>
#include <math/AABB.h>
#include <math/BoundingSphere.h>
#include <math/OBB.h>

namespace df3d {

class MeshData;

class DF3D_DLL StaticMeshComponentProcessor : utils::NonCopyable
{
    struct Impl;
    unique_ptr<Impl> m_pimpl;

public:
    StaticMeshComponentProcessor();
    ~StaticMeshComponentProcessor();

    shared_ptr<MeshData> getMeshData(ComponentInstance comp) const;

    AABB getAABB(ComponentInstance comp);
    BoundingSphere getBoundingSphere(ComponentInstance comp);
    OBB getOBB(ComponentInstance comp);

    void setVisible(ComponentInstance comp, bool visible);
    void disableFrustumCulling(ComponentInstance comp, bool disable);

    ComponentInstance add(Entity e, const std::string &meshFilePath, ResourceLoadingMode lm = ResourceLoadingMode::ASYNC);
    void remove(Entity e);
    ComponentInstance lookup(Entity e);
};

}
