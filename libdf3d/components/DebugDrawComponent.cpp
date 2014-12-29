#include "df3d_pch.h"
#include "DebugDrawComponent.h"

namespace df3d { namespace components {

void DebugDrawComponent::onEvent(components::ComponentEvent ev)
{

}

void DebugDrawComponent::onDraw(render::RenderQueue *ops)
{

}

DebugDrawComponent::DebugDrawComponent()
    : NodeComponent(DEBUG_DRAW)
{
}

DebugDrawComponent::DebugDrawComponent(Type type)
    : DebugDrawComponent()
{
}

DebugDrawComponent::~DebugDrawComponent()
{
}

shared_ptr<NodeComponent> DebugDrawComponent::clone() const
{
    // TODO:
    assert(false);
    return nullptr;
}

} }