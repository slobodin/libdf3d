#pragma once

#include <scene/Entity.h>
#include <scene/EntityComponentProcessor.h>
#include <math/AABB.h>
#include <math/BoundingSphere.h>
#include <math/OBB.h>

namespace df3d {

class MeshData;
class RenderQueue;

class DF3D_DLL StaticMeshComponentProcessor : public EntityComponentProcessor
{
    struct Impl;
    unique_ptr<Impl> m_pimpl;

    void draw(RenderQueue *ops) override;
    void cleanStep(World &w) override;

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
