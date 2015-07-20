#include "df3d_pch.h"
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

    DebugDrawAABBNode()
    {
        m_ro.worldTransform = glm::mat4();
        m_ro.passProps = render::RenderPass::createDebugDrawPass();

        m_ro.vertexData = make_shared<render::VertexBuffer>(render::VertexFormat::create("p:3, tx:2, c:4"));
        m_ro.indexData = make_shared<render::IndexBuffer>();

        m_ro.vertexData->setUsageType(render::GpuBufferUsageType::STREAM);
        m_ro.indexData->setUsageType(render::GpuBufferUsageType::STATIC);

        m_ro.vertexData->resize(8);

        // Fill index buffer.
        auto &indices = m_ro.indexData->getIndices();
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
    }

    ~DebugDrawAABBNode()
    {

    }

    void recreate(const scene::AABB &aabb)
    {
        if (!aabb.isValid())
            return;

        // Create debug visual for AABB.
        // FIXME: This happens even if AABB is the same as was on the last frame.
        render::Vertex_3p2tx4c v;
        auto &minpt = aabb.minPoint();
        auto &maxpt = aabb.maxPoint();
        auto vertices = m_ro.vertexData->getVertexData();
        auto stride = m_ro.vertexData->getFormat().getVertexSize() / sizeof(float);

        v.p.x = minpt.x;
        v.p.y = maxpt.y;
        v.p.z = maxpt.z;
        memcpy(vertices + 0 * stride, &v, sizeof(v));

        v.p.x = minpt.x;
        v.p.y = maxpt.y;
        v.p.z = minpt.z;
        memcpy(vertices + 1 * stride, &v, sizeof(v));

        v.p.x = maxpt.x;
        v.p.y = maxpt.y;
        v.p.z = minpt.z;
        memcpy(vertices + 2 * stride, &v, sizeof(v));

        v.p.x = maxpt.x;
        v.p.y = maxpt.y;
        v.p.z = maxpt.z;
        memcpy(vertices + 3 * stride, &v, sizeof(v));

        v.p.x = minpt.x;
        v.p.y = minpt.y;
        v.p.z = maxpt.z;
        memcpy(vertices + 4 * stride, &v, sizeof(v));

        v.p.x = minpt.x;
        v.p.y = minpt.y;
        v.p.z = minpt.z;
        memcpy(vertices + 5 * stride, &v, sizeof(v));

        v.p.x = maxpt.x;
        v.p.y = minpt.y;
        v.p.z = minpt.z;
        memcpy(vertices + 6 * stride, &v, sizeof(v));

        v.p.x = maxpt.x;
        v.p.y = minpt.y;
        v.p.z = maxpt.z;
        memcpy(vertices + 7 * stride, &v, sizeof(v));

        m_ro.vertexData->setDirty();
    }
};

void DebugDrawComponent::onComponentEvent(components::ComponentEvent ev)
{

}

void DebugDrawComponent::onDraw(render::RenderQueue *ops)
{
    if (!m_holder->mesh())
        return;

    if (!m_debugDraw)
        m_debugDraw = make_unique<DebugDrawAABBNode>();

    auto aabb = m_holder->mesh()->getAABB();
    m_debugDraw->recreate(aabb);
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
