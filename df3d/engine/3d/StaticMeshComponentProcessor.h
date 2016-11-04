#pragma once

#include "SceneGraphComponentProcessor.h"
#include <df3d/game/Entity.h>
#include <df3d/game/EntityComponentProcessor.h>
#include <df3d/engine/render/Material.h>
#include <df3d/engine/resources/MeshResource.h>
#include <df3d/lib/math/AABB.h>
#include <df3d/lib/math/BoundingSphere.h>

namespace df3d {

class RenderQueue;
class World;

class StaticMeshComponentProcessor : public EntityComponentProcessor
{
    friend class World;

    struct Data
    {
        Transform holderWorldTransform;
        Entity holder;
        bool visible = true;
        bool frustumCullingDisabled = false;
        std::vector<MeshPart> parts;
        std::vector<Material> materials;
        AABB localAABB;
        BoundingSphere localBoundingSphere;
    };

    ComponentDataHolder<Data> m_data;

    World &m_world;
    bool m_renderingEnabled = true;

    BoundingSphere getBoundingSphere(const Data &compData);

    void update() override;
    void draw(RenderQueue *ops) override;

public:
    StaticMeshComponentProcessor(World &world);
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
