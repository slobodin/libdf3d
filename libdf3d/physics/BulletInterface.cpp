#include "BulletInterface.h"

#include <base/Service.h>
#include <render/RenderPass.h>
#include <render/RendererBackend.h>
#include <render/VertexIndexBuffer.h>

namespace df3d {

BulletDebugDraw::BulletDebugDraw()
    : m_vertexData(VertexFormat({ VertexFormat::POSITION_3, VertexFormat::TX_2, VertexFormat::COLOR_4 }))
{

}

BulletDebugDraw::~BulletDebugDraw()
{

}

void BulletDebugDraw::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
    // FIXME: map directly to GPU.
    auto v1 = m_vertexData.allocVertex();
    v1.setColor({ color.x(), color.y(), color.z(), 1.0f });
    v1.setPosition({ from.x(), from.y(), from.z() });

    auto v2 = m_vertexData.allocVertex();
    v2.setColor({ color.x(), color.y(), color.z(), 1.0f });
    v2.setPosition({ to.x(), to.y(), to.z() });
}

void BulletDebugDraw::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
{

}

void BulletDebugDraw::reportErrorWarning(const char *warningString)
{
    glog << "Bullet physics:" << warningString << logwarn;
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
    if (!m_pass)
    {
        m_pass = make_shared<RenderPass>(RenderPass::createDebugDrawPass());
        m_pass->setDiffuseColor(1.0f, 1.0f, 1.0f, 0.7f);
    }

    RenderOperation op;
    op.passProps = m_pass;
    op.type = RenderOperation::Type::LINES;

    op.vertexData = make_shared<VertexBuffer>(m_vertexData.getFormat());
    op.vertexData->alloc(m_vertexData, GpuBufferUsageType::STREAM);
    m_vertexData.clear();

    svc().renderMgr.drawOperation(op);
}

}
