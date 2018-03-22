#pragma once

#include <df3d/game/EntityComponentProcessor.h>

namespace df3d {

class AnimatedMeshComponentProcessor : public EntityComponentProcessor
{
    struct Data
    {
    };

    ComponentDataHolder<Data> m_data;

    void update() override;
    void draw(RenderQueue *ops) override;

public:
    AnimatedMeshComponentProcessor();
    ~AnimatedMeshComponentProcessor();

    void add(Entity e, Id meshResource);
    void remove(Entity e) override;
    bool has(Entity e) override;
};

}
