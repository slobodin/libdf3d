#pragma once

namespace df3d {

class Material;

class DF3D_DLL WorldRenderingParams
{
    shared_ptr<Material> m_postProcessMaterial;

    glm::vec3 m_ambientLight = glm::vec3(1.0f, 1.0f, 1.0f);
    float m_fogDensity = 0.0f;
    glm::vec3 m_fogColor;

public:
    WorldRenderingParams();
    ~WorldRenderingParams();

    void setAmbientLight(float ra, float ga, float ba);
    glm::vec3 getAmbientLight() const;

    void setFog(float density, const glm::vec3 &color);
    void setFog(float density, float r, float g, float b);
    float getFogDensity() const;
    glm::vec3 getFogColor() const;

    void setPostProcessMaterial(shared_ptr<Material> material);
    shared_ptr<Material> getPostProcessMaterial() const;
};

}
