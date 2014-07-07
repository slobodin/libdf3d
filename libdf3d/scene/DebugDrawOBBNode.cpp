#include "df3d_pch.h"
#include "DebugDrawOBBNode.h"

/*
#include "RenderOperation.h"
#include "bounding_volumes/OBB.h"
#include "VertexIndexBuffer.h"
#include "RenderPass.h"

namespace df3d { namespace scene {

DebugDrawOBBNode::DebugDrawOBBNode()
{
    m_ro.worldTransform = glm::mat4();
    m_ro.passProps = render::RenderPass::createDebugDrawPass();

    m_ro.vertexData = make_shared<render::VertexBuffer>(render::VertexFormat::create("p:3, tx:2, c:4"));
    m_ro.indexData = make_shared<render::IndexBuffer>();

    m_ro.vertexData->setUsageType(render::GB_USAGE_STATIC);
    m_ro.indexData->setUsageType(render::GB_USAGE_STATIC);

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

DebugDrawOBBNode::~DebugDrawOBBNode()
{
}

void DebugDrawOBBNode::recreate(const OBB &obb)
{
    if (!obb.isValid())
        return;

    // Create debug visual for OBB.
    render::Vertex_3p2tx4c v;
    auto &minpt = obb.minPoint();
    auto &maxpt = obb.maxPoint();
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

void DebugDrawOBBNode::setTransformation(const glm::mat4 &tr)
{
    Node::setTransformation(tr);
    m_ro.worldTransform = m_transformation;
}

shared_ptr<Node> DebugDrawOBBNode::clone() const
{
    auto retRes = make_shared<DebugDrawOBBNode>();

    // Clone base Node.
    cloneTo(retRes);

    // Clone debug draw node fields.
    retRes->m_ro = m_ro;
    // Also, create new vertex buffer (share index buffer).
    retRes->m_ro.vertexData = make_shared<render::VertexBuffer>(m_ro.vertexData->getFormat());
    retRes->m_ro.vertexData->setUsageType(render::GB_USAGE_STATIC);
    retRes->m_ro.vertexData->resize(8);

    return retRes;
}

} }*/