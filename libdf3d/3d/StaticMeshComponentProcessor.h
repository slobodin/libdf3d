#pragma once

#include <game/Entity.h>
#include <game/EntityComponentProcessor.h>
#include <math/AABB.h>
#include <math/BoundingSphere.h>
#include <math/OBB.h>

namespace df3d {

class MeshData;
class RenderQueue;

class DF3D_DLL StaticMeshComponentProcessor : public EntityComponentProcessor
{
    friend class World;

    struct Impl;
    unique_ptr<Impl> m_pimpl;

    void update() override;
    void draw(RenderQueue *ops);
    void cleanStep(const std::list<Entity> &deleted) override;

public:
    StaticMeshComponentProcessor();
    ~StaticMeshComponentProcessor();

    shared_ptr<MeshData> getMeshData(Entity e) const;

    // TODO_ecs: mb use just entity??
    AABB getAABB(Entity e);
    BoundingSphere getBoundingSphere(Entity e);
    OBB getOBB(Entity e);

    void setVisible(Entity e, bool visible);
    void disableFrustumCulling(Entity e, bool disable);

    void add(Entity e, const std::string &meshFilePath, ResourceLoadingMode lm = ResourceLoadingMode::ASYNC);
    void remove(Entity e);
};

}
