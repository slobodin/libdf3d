#include "df3d_pch.h"
#include "BulletInterface.h"

#include <base/SystemsMacro.h>
#include <render/RenderPass.h>
#include <render/RendererBackend.h>
#include <render/VertexIndexBuffer.h>

namespace df3d { namespace physics {

BulletDebugDraw::BulletDebugDraw()
    : m_pass(make_shared<render::RenderPass>(render::RenderPass::createDebugDrawPass()))
{
    m_pass->setDiffuseColor(1.0f, 1.0f, 1.0f, 0.7f);
}

BulletDebugDraw::~BulletDebugDraw()
{

}

void BulletDebugDraw::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
    render::Vertex_3p2tx4c v;
    v.color.r = color.x();
    v.color.g = color.y();
    v.color.b = color.z();

    v.p.x = from.x();
    v.p.y = from.y();
    v.p.z = from.z();
    m_vertexData.push_back(v);

    v.p.x = to.x();
    v.p.y = to.y();
    v.p.z = to.z();
    m_vertexData.push_back(v);
}

void BulletDebugDraw::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
{

}

void BulletDebugDraw::reportErrorWarning(const char *warningString)
{
    base::glog << "Bullet physics:" << warningString << base::logwarn;
}

void BulletDebugDraw::draw3dText(const btVector3 &location, const char *textString)
{

}

void BulletDebugDraw::setDebugMode(int debugMode)
{

}

int BulletDebugDraw::getDebugMode() const
{
    return btIDebugDraw::DBG_DrawAabb | btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawNormals | btIDebugDraw::DBG_DrawConstraints;
}

void BulletDebugDraw::flushRenderOperations()
{
    render::RenderOperation op;
    op.passProps = m_pass;
    op.type = render::RenderOperation::Type::LINES;

    op.vertexData = make_shared<render::VertexBuffer>(render::VertexFormat::create("p:3, tx:2, c:4"));
    op.vertexData->alloc(m_vertexData.size(), m_vertexData.data(), render::GpuBufferUsageType::STREAM);
    m_vertexData.clear();

    g_renderManager->drawOperation(op);
}

} }
