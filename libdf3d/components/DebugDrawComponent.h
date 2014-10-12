#pragma once

#include "NodeComponent.h"

namespace df3d { namespace components {

class DF3D_DLL DebugDrawComponent : public NodeComponent
{
public:
    enum Type
    {
        TYPE_AABB,
        TYPE_OBB
    };

protected:
    void onEvent(components::Event ev);
    void onDraw(render::RenderQueue *ops);

    DebugDrawComponent();

public:
    DebugDrawComponent(Type type);
    ~DebugDrawComponent();

    shared_ptr<NodeComponent> clone() const;
};

} }