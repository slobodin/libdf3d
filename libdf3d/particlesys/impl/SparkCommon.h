#pragma once

#include <SPARK.h>

namespace df3d {

class Texture;
class RenderPass;

namespace particlesys_impl {

class ParticleSystemBuffers_Quad;

class ParticleSystemRenderer : public SPK::Renderer
{
    spark_description(ParticleSystemRenderer, SPK::Renderer)();

public:
    shared_ptr<RenderPass> m_pass;

    // This is a workaround.
    // We need to pass RenderQueue in order to populate it when renderParticles called.
    glm::mat4 *m_currentTransformation;
    ParticleSystemBuffers_Quad *m_quadBuffers;

    ParticleSystemRenderer(bool NEEDS_DATASET);
    ~ParticleSystemRenderer();

    void setBlendMode(SPK::BlendMode blendMode) override;
    void setDiffuseMap(shared_ptr<Texture> texture);
    void enableFaceCulling(bool enable);
};

inline SPK::Vector3D glmToSpk(const glm::vec3 &v)
{
    return SPK::Vector3D(v.x, v.y, v.z);
}

inline glm::vec3 spkToGlm(const SPK::Vector3D &v)
{
    return glm::vec3(v.x, v.y, v.z);
}

} }
