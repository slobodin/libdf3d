#include "df3d_pch.h"
#include "DebugDrawComponent.h"

namespace df3d { namespace components {

void DebugDrawComponent::onEvent(components::Event ev)
{

}

void DebugDrawComponent::onDraw(render::RenderQueue *ops)
{

}

DebugDrawComponent::DebugDrawComponent()
	: NodeComponent(CT_DEBUG_DRAW)
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