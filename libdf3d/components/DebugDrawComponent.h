#pragma once

#include "NodeComponent.h"

namespace df3d {

class DebugDrawAABBNode;

class DF3D_DLL DebugDrawComponent : public NodeComponent
{
public:
    enum class Type
    {
        AABB,
        OBB
    };

private:
    unique_ptr<DebugDrawAABBNode> m_debugDraw;

    void onComponentEvent(ComponentEvent ev) override;
    void onDraw(RenderQueue *ops) override;

    DebugDrawComponent();

public:
    DebugDrawComponent(Type type);
    ~DebugDrawComponent();

    shared_ptr<NodeComponent> clone() const override;
};

}
