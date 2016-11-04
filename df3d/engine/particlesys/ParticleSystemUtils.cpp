#include "ParticleSystemUtils.h"

#include "SparkQuadRenderer.h"
#include "SparkLineTrailRenderer.h"
#include <df3d/engine/EngineController.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/TextureResource.h>

namespace df3d {

SPK::Ref<SPK::Renderer> ParticleSystemUtils::createQuadRenderer(const glm::vec2 &scale, ResourceID textureResource, bool depthTest)
{
    auto quadRenderer = QuadParticleSystemRenderer::create(scale.x, scale.y);

    auto resource = svc().resourceManager().getResource<TextureResource>(textureResource);
    DF3D_ASSERT(resource != nullptr);
    quadRenderer->m_pass.setParam("diffuseMap", resource->handle);
    quadRenderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
    quadRenderer->m_pass.depthTest = depthTest;

    return quadRenderer;
}

SPK::Ref<SPK::Renderer> ParticleSystemUtils::createTrailRenderer(size_t nbSamples, float duration, float width, bool depthTest)
{
    return TrailsParticleSystemRenderer::create(nbSamples, duration, width);
}

}
