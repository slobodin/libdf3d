#include "WorldRenderingParams.h"

#include <libdf3d/utils/Utils.h>

namespace df3d {

WorldRenderingParams::WorldRenderingParams()
{

}

WorldRenderingParams::~WorldRenderingParams()
{

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

void WorldRenderingParams::setPostProcessMaterial(shared_ptr<Material> material)
{
    m_postProcessMaterial = material;
}

shared_ptr<Material> WorldRenderingParams::getPostProcessMaterial() const
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
