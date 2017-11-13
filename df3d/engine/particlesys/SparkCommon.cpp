#include "SparkCommon.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/render/RenderManager.h>

namespace df3d {

ParticleSystemRenderer::ParticleSystemRenderer(bool NEEDS_DATASET)
    : SPK::Renderer(NEEDS_DATASET)
{
    m_pass.setParam(Id("material_diffuse"), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    auto &embedResources = svc().renderManager().getEmbedResources();
    m_pass.setParam(Id("diffuseMap"), embedResources.whiteTexture);
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
        m_pass.setBlending(Blending::NONE);
        break;
    case SPK::BLEND_MODE_ADD:
        m_pass.setBlending(Blending::ADDALPHA);
        break;
    case SPK::BLEND_MODE_ALPHA:
        m_pass.setBlending(Blending::ALPHA);
        break;
    default:
        break;
    }
}

void ParticleSystemRenderer::setDiffuseMap(TextureHandle texture)
{
    m_pass.setParam(Id("diffuseMap"), texture);
}

void ParticleSystemRenderer::enableFaceCulling(bool enable)
{
    m_pass.setBackFaceCullingEnabled(enable);
}

}
