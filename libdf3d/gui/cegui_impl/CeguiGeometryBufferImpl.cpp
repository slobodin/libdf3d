#include "df3d_pch.h"
#include "CeguiGeometryBufferImpl.h"

#include "CeguiTextureImpl.h"
#include "CeguiRendererImpl.h"
#include <base/Controller.h>
#include <render/RenderManager.h>
#include <render/Renderer.h>
#include <render/RenderOperation.h>
#include <render/VertexIndexBuffer.h>

namespace df3d { namespace gui { namespace cegui_impl {

using namespace CEGUI;

render::RenderPass::BlendingMode convertBlendingMode(CEGUI::BlendMode bm)
{
    switch (bm)
    {
    case CEGUI::BM_NORMAL:
        return render::RenderPass::BM_ALPHA;
    case CEGUI::BM_RTT_PREMULTIPLIED:
        return render::RenderPass::BM_ADDALPHA;
    case CEGUI::BM_INVALID:
    default:
        break;
    }

    return render::RenderPass::BM_NONE;
}

glm::mat4 CeguiGeometryBufferImpl::getMatrix() const
{
    if (m_matrixDirty) 
    {

        m_matrixDirty = false;
    }

    return m_op->worldTransform;
}

CeguiGeometryBufferImpl::CeguiGeometryBufferImpl(CeguiRendererImpl &owner)
    : m_owner(owner),
    m_op(new render::RenderOperation())
{
    m_op->vertexData = make_shared<render::VertexBuffer>(render::VertexFormat::create("p:2, tx:2, c:4"));
}

CeguiGeometryBufferImpl::~CeguiGeometryBufferImpl()
{

}

void CeguiGeometryBufferImpl::draw() const
{
    m_op->worldTransform = getMatrix();

    m_op->passProps = m_owner.getDefaultRenderPass();
    m_op->passProps->setBlendMode(convertBlendingMode(d_blendMode));
    if (m_texture)
        m_op->passProps->setSampler("diffuseMap", m_texture->getDf3dTexture());
    else
        m_op->passProps->setSampler("diffuseMap", nullptr);

    g_renderManager->getRenderer()->enableScissorTest(m_clippingActive);
    g_renderManager->getRenderer()->setScissorRegion(m_clippingRegion.left(), m_clippingRegion.top(), m_clippingRegion.right(), m_clippingRegion.bottom());
}

void CeguiGeometryBufferImpl::setTranslation(const Vector3f &v)
{
    m_translation = v;
    m_matrixDirty = true;
}

void CeguiGeometryBufferImpl::setRotation(const Quaternion &r)
{
    m_rotation = r;
    m_matrixDirty = true;
}

void CeguiGeometryBufferImpl::setPivot(const Vector3f &p)
{
    m_pivot = p;
    m_matrixDirty = true;
}

void CeguiGeometryBufferImpl::setClippingRegion(const Rectf &region)
{
    m_clippingRegion = region;
}

void CeguiGeometryBufferImpl::appendVertex(const Vertex &vertex)
{
    appendGeometry(&vertex, 1);
}

void CeguiGeometryBufferImpl::appendGeometry(const Vertex *const vbuff, uint vertex_count)
{

}

void CeguiGeometryBufferImpl::setActiveTexture(Texture *texture)
{
    m_texture = static_cast<CeguiTextureImpl *>(texture);
}

void CeguiGeometryBufferImpl::reset()
{
    m_op->vertexData->clear();
    m_op->vertexData->setDirty();
    m_texture = nullptr;
}

Texture* CeguiGeometryBufferImpl::getActiveTexture() const
{
    return m_texture;
}

uint CeguiGeometryBufferImpl::getVertexCount() const
{
    return 0;
}

uint CeguiGeometryBufferImpl::getBatchCount() const
{
    return 0;
}

void CeguiGeometryBufferImpl::setRenderEffect(RenderEffect *effect)
{

}

RenderEffect* CeguiGeometryBufferImpl::getRenderEffect()
{
    return nullptr;
}

void CeguiGeometryBufferImpl::setClippingActive(const bool active)
{
    m_clippingActive = active;
}

bool CeguiGeometryBufferImpl::isClippingActive() const
{
    return m_clippingActive;
}

} } }