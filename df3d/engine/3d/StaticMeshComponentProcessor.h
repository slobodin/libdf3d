#pragma once

#include <df3d/game/Entity.h>
#include <df3d/game/EntityComponentProcessor.h>
#include <df3d/lib/math/AABB.h>
#include <df3d/lib/math/BoundingSphere.h>
#include <df3d/lib/math/OBB.h>

namespace df3d {

class MeshData;
class RenderQueue;
class World;

class DF3D_DLL StaticMeshComponentProcessor : public EntityComponentProcessor
{
    friend class World;

    struct Impl;
    unique_ptr<Impl> m_pimpl;

    World *m_world;
    bool m_renderingEnabled = true;

    void update() override;
    void draw(RenderQueue *ops) override;

public:
    StaticMeshComponentProcessor(World *world);
    ~StaticMeshComponentProcessor();

    shared_ptr<MeshData> getMeshData(Entity e) const;

    AABB getAABB(Entity e);
    BoundingSphere getBoundingSphere(Entity e);
    OBB getOBB(Entity e);

    void enableRender(bool enable);
    void setVisible(Entity e, bool visible);
    void disableFrustumCulling(Entity e, bool disable);

    bool isVisible(Entity e);

    void add(Entity e, const std::string &meshFilePath);
    void add(Entity e, const std::string &meshFilePath, ResourceLoadingMode lm);
    void add(Entity e, shared_ptr<MeshData> meshData);

    void remove(Entity e) override;
    bool has(Entity e) override;
};

}
