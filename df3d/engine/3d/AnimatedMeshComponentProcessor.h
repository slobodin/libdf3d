#pragma once

#include <df3d/game/EntityComponentProcessor.h>
#include <df3d/game/ComponentDataHolder.h>
#include <df3d/engine/render/Material.h>
#include <df3d/engine/resources/MeshResource.h>
#include <df3d/engine/3d/SceneGraphComponentProcessor.h>

namespace df3d {

class World;

class AnimatedMeshComponentProcessor : public EntityComponentProcessor
{
    struct Data
    {
        Transform holderWorldTransform;
        std::vector<MeshPart> meshParts;
        std::vector<Material> materials;
        shared_ptr<AnimatedMeshNode> root;
        int64_t frameCounter = 0;
        df3d::Entity holder;
        float timer = 0.0f;
    };

    ComponentDataHolder<Data> m_data;
    World &m_world;

    void drawNode(AnimatedMeshNode *node, RenderQueue *ops, Data &data, glm::mat4 parentTransform);

    void update() override;
    void draw(RenderQueue *ops) override;

public:
    AnimatedMeshComponentProcessor(World &world);
    ~AnimatedMeshComponentProcessor();

    void add(Entity e, Id meshResource);
    void remove(Entity e) override;
    bool has(Entity e) override;
};

}
