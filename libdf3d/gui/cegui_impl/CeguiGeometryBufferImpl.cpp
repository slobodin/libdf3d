#include "df3d_pch.h"
#include "CeguiGeometryBufferImpl.h"

#include "CeguiTextureImpl.h"
#include "CeguiRendererImpl.h"
#include <base/Controller.h>
#include <render/RenderManager.h>
#include <render/Renderer.h>
#include <render/RenderOperation.h>
#include <render/VertexIndexBuffer.h>
#include <render/RenderPass.h>
#include <render/GpuProgram.h>

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

const glm::mat4 &CeguiGeometryBufferImpl::getMatrix() const
{
    if (m_matrixDirty) 
    {
        m_matrix = glm::mat4(1.0f);

        const glm::vec3 finalTrans(m_translation.d_x + m_pivot.d_x, m_translation.d_y + m_pivot.d_y, m_translation.d_z + m_pivot.d_z);

        m_matrix = glm::translate(m_matrix, finalTrans);

        glm::quat rotationQuat = glm::quat(m_rotation.d_w, m_rotation.d_x, m_rotation.d_y, m_rotation.d_z);
        glm::mat4 rotationMatrix = glm::mat4_cast(rotationQuat);

        m_matrix = m_matrix * rotationMatrix;

        glm::vec3 transl = glm::vec3(-m_pivot.d_x, -m_pivot.d_y, -m_pivot.d_z);
        glm::mat4 translMatrix = glm::translate(glm::mat4(1.f), transl);
        m_matrix = m_matrix * translMatrix;

        m_matrixDirty = false;
    }

    return m_matrix;
}

void CeguiGeometryBufferImpl::setupScissorRegion(bool active) const
{
    g_renderManager->getRenderer()->enableScissorTest(active);

    if (active)
    {
        const auto &viewport = m_owner.getActiveRenderTarget()->getArea();

        auto scissorX = static_cast<int>(m_clippingRegion.left());
        auto scissorY = static_cast<int>(viewport.getHeight() - m_clippingRegion.bottom());
        auto scissorWidth = static_cast<int>(m_clippingRegion.getWidth());
        auto scissorHeight = static_cast<int>(m_clippingRegion.getHeight());
        
        g_renderManager->getRenderer()->setScissorRegion(scissorX, scissorY, scissorWidth, scissorHeight);
    }
}

CeguiGeometryBufferImpl::CeguiGeometryBufferImpl(CeguiRendererImpl &owner)
    : m_owner(owner)
{
    
}

CeguiGeometryBufferImpl::~CeguiGeometryBufferImpl()
{
    reset();
}

void CeguiGeometryBufferImpl::draw() const
{
    const auto &matrix = getMatrix();

    const int passCount = m_effect ? m_effect->getPassCount() : 1;
    size_t pos = 0;
    for (int pass = 0; pass < passCount; ++pass)
    {
        if (m_effect)
            m_effect->performPreRenderFunctions(pass);

        for (const auto &batch : m_batches)
        {
            setupScissorRegion(batch.clippingActive);

            batch.m_op->worldTransform = matrix;
            batch.m_op->passProps->setBlendMode(convertBlendingMode(d_blendMode));

            g_renderManager->drawOperation(*batch.m_op);
        }
    }

    if (m_effect)
        m_effect->performPostRenderFunctions();
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
    m_clippingRegion.top(ceguimax(0.0f, region.top()));
    m_clippingRegion.left(ceguimax(0.0f, region.left()));
    m_clippingRegion.bottom(ceguimax(0.0f, region.bottom()));
    m_clippingRegion.right(ceguimax(0.0f, region.right()));
}

void CeguiGeometryBufferImpl::appendVertex(const Vertex &vertex)
{
    appendGeometry(&vertex, 1);
}

void CeguiGeometryBufferImpl::appendGeometry(const Vertex *const vbuff, uint vertex_count)
{
    bool addNewOperation = m_batches.empty();
    if (!addNewOperation)
    {
        auto &currBatch = m_batches.back();
        auto currTexture = currBatch.m_op->passProps->getSampler("diffuseMap");

        if (currTexture.get() != m_activeTexture->getDf3dTexture().get())
            addNewOperation = true;
        if (currBatch.clippingActive != m_clippingActive)
            addNewOperation = true;
    }

    if (addNewOperation)
    {
        Batch newBatch;

        auto renderPass = make_shared<render::RenderPass>();
        renderPass->setFaceCullMode(render::RenderPass::FCM_NONE);
        renderPass->setGpuProgram(render::GpuProgram::createFFP2DGpuProgram());
        renderPass->enableDepthTest(false);
        renderPass->enableDepthWrite(false);
        renderPass->setBlendMode(render::RenderPass::BM_ALPHA);

        if (m_activeTexture)
            renderPass->setSampler("diffuseMap", m_activeTexture->getDf3dTexture());
        else
            renderPass->setSampler("diffuseMap", nullptr);

        newBatch.m_op = new render::RenderOperation();
        newBatch.m_op->passProps = renderPass;
        newBatch.m_op->vertexData = make_shared<render::VertexBuffer>(render::VertexFormat::create("p:3, tx:2, c:4"));
        newBatch.m_op->vertexData->setUsageType(render::GB_USAGE_DYNAMIC);
        newBatch.clippingActive = m_clippingActive;

        m_batches.push_back(newBatch);
    }

    // Copy over vertices to the current batch.
    const Vertex *vs = vbuff;
    Batch &currentBatch = m_batches.back();
    currentBatch.m_op->vertexData->setDirty();
    
    for (size_t i = 0; i < vertex_count; i++, vs++)
    {
        render::Vertex_3p2tx4c v;
        v.p.x = vs->position.d_x;
        v.p.y = vs->position.d_y;
        v.p.z = vs->position.d_z;
        v.tx.x = vs->tex_coords.d_x;
        v.tx.y = vs->tex_coords.d_y;
        v.color.r = vs->colour_val.getRed();
        v.color.g = vs->colour_val.getGreen();
        v.color.b = vs->colour_val.getBlue();
        v.color.a = vs->colour_val.getAlpha();

        currentBatch.m_op->vertexData->appendVertexData((const float *)&v, 1);
    }
}

void CeguiGeometryBufferImpl::setActiveTexture(Texture *texture)
{
    m_activeTexture = static_cast<CeguiTextureImpl *>(texture);
}

void CeguiGeometryBufferImpl::reset()
{
    for (auto &batch : m_batches)
        delete batch.m_op;

    m_batches.clear();
    m_activeTexture = nullptr;
}

Texture* CeguiGeometryBufferImpl::getActiveTexture() const
{
    return m_activeTexture;
}

uint CeguiGeometryBufferImpl::getVertexCount() const
{
    auto count = std::accumulate(m_batches.begin(), m_batches.end(), 0, [](int res, const Batch &b) { return res += b.m_op->vertexData->getVerticesCount(); });
    return count;
}

uint CeguiGeometryBufferImpl::getBatchCount() const
{
    return m_batches.size();
}

void CeguiGeometryBufferImpl::setRenderEffect(RenderEffect *effect)
{
    m_effect = effect;
}

RenderEffect* CeguiGeometryBufferImpl::getRenderEffect()
{
    return m_effect;
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