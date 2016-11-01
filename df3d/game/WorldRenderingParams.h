#pragma once

#include <df3d/engine/3d/Light.h>

namespace df3d {

class Material;

class WorldRenderingParams
{
    Material *m_postProcessMaterial = nullptr;

    glm::vec3 m_ambientLight = glm::vec3(1.0f, 1.0f, 1.0f);
    float m_fogDensity = 0.0f;
    glm::vec3 m_fogColor;

    std::vector<Light> m_lights;

public:
    WorldRenderingParams();
    ~WorldRenderingParams();

    void setAmbientLight(float ra, float ga, float ba);
    const glm::vec3& getAmbientLight() const;

    void setFog(float density, const glm::vec3 &color);
    void setFog(float density, float r, float g, float b);
    float getFogDensity() const;
    const glm::vec3& getFogColor() const;

    void setPostProcessMaterial(const Material &material);
    const Material* getPostProcessMaterial() const;

    void addLight(const Light &light) { m_lights.push_back(light); }
    const std::vector<Light>& getLights() const { return m_lights; }
    std::vector<Light>& getLights() { return m_lights; }
    Light* getLightByName(const std::string &name);
};

}
