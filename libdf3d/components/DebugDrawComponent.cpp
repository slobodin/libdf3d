#include "DebugDrawComponent.h"

#include "MeshComponent.h"
#include "TransformComponent.h"
#include <scene/Node.h>
#include <render/RenderOperation.h>
#include <render/RenderQueue.h>
#include <render/RenderPass.h>
#include <render/VertexIndexBuffer.h>

namespace df3d { namespace components {

class DebugDrawAABBNode
{
public:
    render::RenderOperation m_ro;
    shared_ptr<render::RenderPass> m_pass;

    DebugDrawAABBNode()
    {
        m_pass = make_shared<render::RenderPass>(render::RenderPass::createDebugDrawPass());

        m_ro.worldTransform = glm::mat4();
        m_ro.passProps = m_pass;

        // Fill index buffer.
        m_ro.indexData = make_shared<render::IndexBuffer>();
        render::IndexArray indices;
        indices.push_back(0); indices.push_back(2); indices.push_back(1);
        indices.push_back(0); indices.push_back(3); indices.push_back(2);
        indices.push_back(0); indices.push_back(1); indices.push_back(5);
        indices.push_back(0); indices.push_back(5); indices.push_back(4);
        indices.push_back(2); indices.push_back(5); indices.push_back(1);
        indices.push_back(2); indices.push_back(6); indices.push_back(5);
        indices.push_back(3); indices.push_back(6); indices.push_back(2);
        indices.push_back(3); indices.push_back(7); indices.push_back(6);
        indices.push_back(4); indices.push_back(5); indices.push_back(6);
        indices.push_back(4); indices.push_back(6); indices.push_back(7);
        indices.push_back(0); indices.push_back(4); indices.push_back(3);
        indices.push_back(4); indices.push_back(7); indices.push_back(3);

        m_ro.indexData->alloc(indices.size(), indices.data(), render::GpuBufferUsageType::STATIC);
    }

    ~DebugDrawAABBNode()
    {

    }

    void recreate(const scene::AABB &aabb)
    {
        if (!aabb.isValid())
            return;

        render::VertexFormat vertexFormat({ render::VertexFormat::POSITION_3, render::VertexFormat::TX_2, render::VertexFormat::COLOR_4 });
        render::VertexData vertexData(vertexFormat);

        // Create debug visual for AABB.
        // FIXME: This happens even if AABB is the same as was on the last frame.
        const auto &minpt = aabb.minPoint();
        const auto &maxpt = aabb.maxPoint();

        // FIXME: set color also.

        vertexData.getNextVertex().setPosition({ minpt.x, maxpt.y, maxpt.z });
        vertexData.getNextVertex().setPosition({ minpt.x, maxpt.y, minpt.z });
        vertexData.getNextVertex().setPosition({ maxpt.x, maxpt.y, minpt.z });
        vertexData.getNextVertex().setPosition({ maxpt.x, maxpt.y, maxpt.z });
        vertexData.getNextVertex().setPosition({ minpt.x, minpt.y, maxpt.z });
        vertexData.getNextVertex().setPosition({ minpt.x, minpt.y, minpt.z });
        vertexData.getNextVertex().setPosition({ maxpt.x, minpt.y, minpt.z });
        vertexData.getNextVertex().setPosition({ maxpt.x, minpt.y, maxpt.z });

        m_ro.vertexData = make_shared<render::VertexBuffer>(vertexFormat);
        m_ro.vertexData->alloc(vertexData, render::GpuBufferUsageType::STREAM);
    }
};

void DebugDrawComponent::onComponentEvent(components::ComponentEvent ev)
{

}

void DebugDrawComponent::onDraw(render::RenderQueue *ops)
{
    assert(false);  // TODO: check it works.

    if (!m_holder->mesh())
        return;

    if (!m_debugDraw)
        m_debugDraw = make_unique<DebugDrawAABBNode>();

    m_debugDraw->recreate(m_holder->mesh()->getAABB());
    ops->debugDrawOperations.push_back(m_debugDraw->m_ro);
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
