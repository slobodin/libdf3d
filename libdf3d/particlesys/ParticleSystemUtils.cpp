#include "ParticleSystemUtils.h"

#include "impl/ParticleSystemLoader.h"
#include "impl/SparkInterface.h"
#include <libdf3d/base/EngineController.h>
#include <libdf3d/resources/ResourceManager.h>
#include <libdf3d/resources/ResourceFactory.h>
#include <libdf3d/render/Texture2D.h>
#include <libdf3d/render/RenderPass.h>
#include <libdf3d/utils/JsonUtils.h>

namespace df3d {

ParticleSystemCreationParams ParticleSystemUtils::parseVfx(const std::string &vfxFile)
{
    auto vfxJson = utils::json::fromFile(vfxFile);
    auto spkSystem = particlesys_impl::ParticleSystemLoader::createSpkSystem(vfxJson);
    if (!spkSystem)
    {
        glog << "Failed to parse particle system definition" << logwarn;
        return ParticleSystemCreationParams();
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

    quadRenderer->setDiffuseMap(svc().resourceManager().getFactory().createTexture(texturePath, ResourceLoadingMode::ASYNC));
    quadRenderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
    quadRenderer->m_pass->enableDepthTest(depthTest);

    return quadRenderer;
}

}
