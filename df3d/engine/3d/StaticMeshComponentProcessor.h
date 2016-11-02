#pragma once

#include <df3d/game/Entity.h>
#include <df3d/game/EntityComponentProcessor.h>
#include <df3d/lib/math/AABB.h>
#include <df3d/lib/math/BoundingSphere.h>

namespace df3d {

class RenderQueue;
class World;
class Material;

class StaticMeshComponentProcessor : public EntityComponentProcessor
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

    void setMaterial(Entity e, const Material &material);
    void setMaterial(Entity e, size_t meshPartIdx, const Material &material);
    Material* getMaterial(Entity e, size_t meshPartIdx);

    AABB getAABB(Entity e);
    BoundingSphere getBoundingSphere(Entity e);

    void enableRender(bool enable);
    void setVisible(Entity e, bool visible);
    void disableFrustumCulling(Entity e, bool disable);

    bool isVisible(Entity e);

    void add(Entity e, ResourceID meshResource);
    void remove(Entity e) override;
    bool has(Entity e) override;
};

}
