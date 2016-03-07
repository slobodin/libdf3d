#include "SparkCommon.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/resources/ResourceManager.h>
#include <libdf3d/resources/ResourceFactory.h>
#include <libdf3d/render/RenderPass.h>

namespace df3d { namespace particlesys_impl {

ParticleSystemRenderer::ParticleSystemRenderer(bool NEEDS_DATASET)
    : SPK::Renderer(NEEDS_DATASET),
    m_pass(make_shared<RenderPass>())
{
    m_pass->setFaceCullMode(RenderPass::FaceCullMode::BACK);
    m_pass->setFrontFaceWinding(RenderPass::WindingOrder::CCW);
    m_pass->setDiffuseColor(1.0f, 1.0f, 1.0f);
    m_pass->setSampler("diffuseMap", std::shared_ptr<Texture>());          // FIXME: force to use default white texture (as using colored program)

    m_pass->setGpuProgram(svc().resourceManager().getFactory().createColoredGpuProgram());
}

ParticleSystemRenderer::~ParticleSystemRenderer()
{

}

void ParticleSystemRenderer::setBlendMode(SPK::BlendMode blendMode)
{
    switch (blendMode)
    {
    case SPK::BLEND_MODE_NONE:
        m_pass->setBlendMode(RenderPass::BlendingMode::NONE);
        break;
    case SPK::BLEND_MODE_ADD:
        m_pass->setBlendMode(RenderPass::BlendingMode::ADDALPHA);
        break;
    case SPK::BLEND_MODE_ALPHA:
        m_pass->setBlendMode(RenderPass::BlendingMode::ALPHA);
        break;
    default:
        break;
    }
}

void ParticleSystemRenderer::setDiffuseMap(shared_ptr<Texture> texture)
{
    m_pass->setSampler("diffuseMap", texture);
}

void ParticleSystemRenderer::enableFaceCulling(bool enable)
{
    if (enable)
    {
        m_pass->setFaceCullMode(RenderPass::FaceCullMode::BACK);
        m_pass->setFrontFaceWinding(RenderPass::WindingOrder::CCW);
    }
    else
    {
        m_pass->setFaceCullMode(RenderPass::FaceCullMode::NONE);
    }
}

} }
