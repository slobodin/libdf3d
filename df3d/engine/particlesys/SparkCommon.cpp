#include "SparkCommon.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceFactory.h>
#include <df3d/engine/render/RenderPass.h>

namespace df3d { namespace particlesys_impl {

ParticleSystemRenderer::ParticleSystemRenderer(bool NEEDS_DATASET)
    : SPK::Renderer(NEEDS_DATASET),
    m_pass(make_shared<RenderPass>())
{
    m_pass->setFaceCullMode(FaceCullMode::BACK);
    m_pass->getPassParam("material_diffuse")->setValue(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    m_pass->getPassParam("diffuseMap")->setValue(nullptr);          // FIXME: force to use default white texture (as using colored program)

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
        m_pass->setBlendMode(BlendingMode::NONE);
        break;
    case SPK::BLEND_MODE_ADD:
        m_pass->setBlendMode(BlendingMode::ADDALPHA);
        break;
    case SPK::BLEND_MODE_ALPHA:
        m_pass->setBlendMode(BlendingMode::ALPHA);
        break;
    default:
        break;
    }
}

void ParticleSystemRenderer::setDiffuseMap(shared_ptr<Texture> texture)
{
    m_pass->getPassParam("diffuseMap")->setValue(texture);
}

void ParticleSystemRenderer::enableFaceCulling(bool enable)
{
    if (enable)
        m_pass->setFaceCullMode(FaceCullMode::BACK);
    else
        m_pass->setFaceCullMode(FaceCullMode::NONE);
}

} }
