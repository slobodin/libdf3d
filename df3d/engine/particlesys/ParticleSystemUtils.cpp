#include "ParticleSystemUtils.h"

#include "ParticleSystemLoader.h"
#include "SparkQuadRenderer.h"
#include "SparkLineTrailRenderer.h"
#include <df3d/engine/EngineController.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceFactory.h>
#include <df3d/engine/render/Texture.h>
#include <df3d/engine/render/RenderPass.h>
#include <df3d/lib/JsonUtils.h>

namespace df3d {

ParticleSystemCreationParams ParticleSystemUtils::parseVfx(const char *vfxFile)
{
    auto vfxJson = JsonUtils::fromFile(vfxFile, svc().fileSystem());
    auto spkSystem = particlesys_impl::ParticleSystemLoader::createSpkSystem(vfxJson);
    if (!spkSystem)
    {
        DFLOG_WARN("Failed to parse particle system definition");
        return{};
    }

    bool worldTransformed = true;
    float systemLifeTime = -1.0f;

    vfxJson["worldTransformed"] >> worldTransformed;
    vfxJson["systemLifeTime"] >> systemLifeTime;

    ParticleSystemCreationParams result;
    result.spkSystem = spkSystem;
    result.worldTransformed = worldTransformed;
    result.systemLifeTime = systemLifeTime;

    return result;
}

SPK::Ref<SPK::Renderer> ParticleSystemUtils::createQuadRenderer(const glm::vec2 &scale, const std::string &texturePath, bool depthTest)
{
    auto quadRenderer = particlesys_impl::QuadParticleSystemRenderer::create(scale.x, scale.y);

    uint32_t flags = TEXTURE_FILTERING_TRILINEAR | TEXTURE_WRAP_MODE_REPEAT;
    quadRenderer->setDiffuseMap(svc().resourceManager().getFactory().createTexture(texturePath, flags, ResourceLoadingMode::ASYNC));
    quadRenderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
    quadRenderer->m_pass->enableDepthTest(depthTest);

    return quadRenderer;
}

SPK::Ref<SPK::Renderer> ParticleSystemUtils::createTrailRenderer(size_t nbSamples, float duration, float width, bool depthTest)
{
    return particlesys_impl::TrailsParticleSystemRenderer::create(nbSamples, duration, width);
}

void ParticleSystemUtils::enableFaceCulling(SPK::Ref<SPK::Renderer> renderer, bool enable)
{
    (dynamic_cast<particlesys_impl::ParticleSystemRenderer*>(renderer.get()))->enableFaceCulling(enable);
}

}
