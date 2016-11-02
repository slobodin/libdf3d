#include "SparkCommon.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/render/RenderManager.h>

namespace df3d { namespace particlesys_impl {

ParticleSystemRenderer::ParticleSystemRenderer(bool NEEDS_DATASET)
    : SPK::Renderer(NEEDS_DATASET)
{
    m_pass.faceCullMode = FaceCullMode::BACK;
    m_pass.setParam("material_diffuse", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    auto &embedResources = svc().renderManager().getEmbedResources();
    m_pass.setParam("diffuseMap", embedResources.whiteTexture);
    m_pass.program = embedResources.coloredProgram;
}

ParticleSystemRenderer::~ParticleSystemRenderer()
{

}

void ParticleSystemRenderer::setBlendMode(SPK::BlendMode blendMode)
{
    switch (blendMode)
    {
    case SPK::BLEND_MODE_NONE:
        m_pass.blendMode = BlendingMode::NONE;
        m_pass.isTransparent = false;
        break;
    case SPK::BLEND_MODE_ADD:
        m_pass.blendMode = BlendingMode::ADDALPHA;
        m_pass.isTransparent = true;
        break;
    case SPK::BLEND_MODE_ALPHA:
        m_pass.blendMode = BlendingMode::ALPHA;
        m_pass.isTransparent = true;
        break;
    default:
        break;
    }
}

void ParticleSystemRenderer::setDiffuseMap(TextureHandle texture)
{
    m_pass.setParam("diffuseMap", texture);
}

void ParticleSystemRenderer::enableFaceCulling(bool enable)
{
    if (enable)
        m_pass.faceCullMode = FaceCullMode::BACK;
    else
        m_pass.faceCullMode = FaceCullMode::NONE;
}

} }
