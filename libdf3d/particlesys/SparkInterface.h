#pragma once

#include <spark/SPARK.h>

FWD_MODULE_CLASS(render, RenderPass)
FWD_MODULE_CLASS(render, RenderQueue)
FWD_MODULE_CLASS(render, Texture)

namespace df3d { namespace particlesys {

class MyRenderBuffer;

class ParticleSystemRenderer : public SPK::Renderer
{
    spark_description(ParticleSystemRenderer, SPK::Renderer)();
public:
    shared_ptr<render::RenderPass> m_pass;

    // This is a workaround. 
    // We need to pass RenderQueue in order to populate it when renderParticles called.
    render::RenderQueue *m_currentRenderQueue;
    glm::mat4 *m_currentTransformation;

    ParticleSystemRenderer(bool NEEDS_DATASET);
    ~ParticleSystemRenderer();

    SPK::RenderBuffer* attachRenderBuffer(const SPK::Group &group) const override;

    void setBlendMode(SPK::BlendMode blendMode) override;
    void setDiffuseMap(shared_ptr<render::Texture> texture);
};

// A Renderer drawing particles as quads.
class QuadParticleSystemRenderer : public ParticleSystemRenderer, public SPK::QuadRenderBehavior, public SPK::Oriented3DRenderBehavior
{
    spark_description(QuadParticleSystemRenderer, ParticleSystemRenderer)();
private:
    mutable void (QuadParticleSystemRenderer::*m_renderParticle)(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const = nullptr;

    // Rendering for particles with texture 2D or no texture.
    void render2D(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const;
    // Rendering for particles with texture 2D or no texture and rotation.
    void render2DRot(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const;

    void fillBufferColorAndVertex(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const;

    QuadParticleSystemRenderer(float scaleX = 1.0f, float scaleY = 1.0f);

public:
    ~QuadParticleSystemRenderer();

    void render(const SPK::Group &group, const SPK::DataSet *dataSet, SPK::RenderBuffer *renderBuffer) const override;
    void computeAABB(SPK::Vector3D &AABBMin, SPK::Vector3D &AABBMax, const SPK::Group &group, const SPK::DataSet *dataSet) const override;

    // Creates and registers a new QuadParticleSystemRenderer
    static SPK::Ref<QuadParticleSystemRenderer> create(float scaleX = 1.0f, float scaleY = 1.0f)
    {
        return SPK_NEW(QuadParticleSystemRenderer, scaleX, scaleY);
    }
};

inline SPK::Vector3D glmToSpk(const glm::vec3 &v)
{
    return SPK::Vector3D(v.x, v.y, v.z);
}

inline glm::vec3 spkToGlm(const SPK::Vector3D &v)
{
    return glm::vec3(v.x, v.y, v.z);
}

void initSparkEngine();
void destroySparkEngine();

} }