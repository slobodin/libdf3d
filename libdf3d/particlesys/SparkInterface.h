#pragma once

#include <spark/SPK.h>

FWD_MODULE_CLASS(render, RenderPass)
FWD_MODULE_CLASS(render, VertexBuffer)
FWD_MODULE_CLASS(render, IndexBuffer)
FWD_MODULE_CLASS(render, Texture)

namespace df3d { namespace particlesys {

class MyBuffer;

class ParticleSystemRenderer : public SPK::Renderer
{
public:
    shared_ptr<render::RenderPass> m_pass;
    mutable MyBuffer *m_currentBuffer = nullptr;

    ParticleSystemRenderer();
    ~ParticleSystemRenderer();

    void setBlending(SPK::BlendingMode blendMode);
    void setDiffuseMap(shared_ptr<render::Texture> texture);

    shared_ptr<render::VertexBuffer> getVertexBuffer() const;
    shared_ptr<render::IndexBuffer> getIndexBuffer() const;
};

class QuadParticleSystemRenderer : public ParticleSystemRenderer, public SPK::QuadRendererInterface, public SPK::Oriented3DRendererInterface
{
    SPK_IMPLEMENT_REGISTERABLE(QuadParticleSystemRenderer)

    bool checkBuffers(const SPK::Group& group);

    void (QuadParticleSystemRenderer::*m_renderParticle)(const SPK::Particle&) const = nullptr;

    // Rendering for particles with texture 2D or no texture.
    void render2D(const SPK::Particle& particle) const;
    // Rendering for particles with texture 2D or no texture and rotation.
    void render2DRot(const SPK::Particle& particle) const;

public:
    QuadParticleSystemRenderer(float scaleX = 1.0f, float scaleY = 1.0f);
    ~QuadParticleSystemRenderer();

    void createBuffers(const SPK::Group& group);
    void destroyBuffers(const SPK::Group& group);

    void render(const SPK::Group& group);

    static QuadParticleSystemRenderer* create(float scaleX = 1.0f, float scaleY = 1.0f);
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