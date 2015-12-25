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

    World *m_world;

    void update() override;
    void draw(RenderQueue *ops);
    void cleanStep(const std::list<Entity> &deleted) override;

public:
    StaticMeshComponentProcessor(World *world);
    ~StaticMeshComponentProcessor();

    shared_ptr<MeshData> getMeshData(Entity e) const;

    AABB getAABB(Entity e);
    BoundingSphere getBoundingSphere(Entity e);
    OBB getOBB(Entity e);

    void setVisible(Entity e, bool visible);
    void disableFrustumCulling(Entity e, bool disable);

    void add(Entity e, const std::string &meshFilePath, ResourceLoadingMode lm = ResourceLoadingMode::ASYNC);
    void add(Entity e, shared_ptr<MeshData> meshData);
    void remove(Entity e);
    bool has(Entity e);
};

}
