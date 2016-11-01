#include "WorldRenderingParams.h"

#include <df3d/lib/Utils.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/render/Material.h>

namespace df3d {

WorldRenderingParams::WorldRenderingParams()
{

}

WorldRenderingParams::~WorldRenderingParams()
{
    MemoryManager::allocDefault().makeDelete(m_postProcessMaterial);
}

void WorldRenderingParams::setAmbientLight(float ra, float ga, float ba)
{
    m_ambientLight.r = utils::clamp(ra, 0.0f, 1.0f);
    m_ambientLight.g = utils::clamp(ga, 0.0f, 1.0f);
    m_ambientLight.b = utils::clamp(ba, 0.0f, 1.0f);
}

const glm::vec3& WorldRenderingParams::getAmbientLight() const
{
    return m_ambientLight;
}

void WorldRenderingParams::setFog(float density, const glm::vec3 &color)
{
    m_fogDensity = density;
    m_fogColor = color;
}

void WorldRenderingParams::setFog(float density, float r, float g, float b)
{
    setFog(density, glm::vec3(r, g, b));
}

float WorldRenderingParams::getFogDensity() const
{
    return m_fogDensity;
}

const glm::vec3& WorldRenderingParams::getFogColor() const
{
    return m_fogColor;
}

void WorldRenderingParams::setPostProcessMaterial(const Material &material)
{
    if (m_postProcessMaterial)
        MemoryManager::allocDefault().makeDelete(m_postProcessMaterial);
    m_postProcessMaterial = MemoryManager::allocDefault().makeNew<Material>(material);
}

const Material* WorldRenderingParams::getPostProcessMaterial() const
{
    return m_postProcessMaterial;
}

Light* WorldRenderingParams::getLightByName(const std::string &name)
{
    if (name.empty())
    {
        DFLOG_WARN("Failed to find a light with empty name");
        return nullptr;
    }

    for (auto &light : m_lights)
    {
        if (light.getName() == name)
            return &light;
    }

    return nullptr;
}

}
