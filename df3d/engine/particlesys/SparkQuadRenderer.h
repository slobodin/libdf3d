#pragma once

#include "SparkCommon.h"

#include <df3d/engine/render/RenderCommon.h>
#include <df3d/engine/render/Vertex.h>

namespace df3d {

class MyRenderBuffer;

// A cache for index data used by all particle systems.
class ParticleSystemIndexBuffer
{
    IndexBufferHandle m_indexBuffer;
    size_t m_particlesAllocated = 0;

    void cleanup();
public:
    ParticleSystemIndexBuffer();
    ~ParticleSystemIndexBuffer();

    void reallocIfNeeded(size_t nbParticles);
    IndexBufferHandle getHandle() const { return m_indexBuffer; }
};

//! A Renderer drawing particles as quads.
class QuadParticleSystemRenderer : public ParticleSystemRenderer, public SPK::QuadRenderBehavior, public SPK::Oriented3DRenderBehavior
{
    spark_description(QuadParticleSystemRenderer, ParticleSystemRenderer)
private:
    mutable void (QuadParticleSystemRenderer::*m_renderParticle)(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const = nullptr;

    //! Rendering for particles with texture 2D or no texture.
    void render2D(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const;
    //! Rendering for particles with texture 2D or no texture and rotation.
    void render2DRot(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const;
    //! Rendering for particles with texture 2D atlas.
    void render2DAtlas(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const;
    //! Rendering for particles with texture 2D atlas and rotation.
    void render2DAtlasRot(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const;

    void fillBufferColorAndVertex(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const;
    void fillBufferTexture2DCoordsAtlas(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const;

    QuadParticleSystemRenderer(float scaleX = 1.0f, float scaleY = 1.0f);

public:
    ~QuadParticleSystemRenderer();

    SPK::RenderBuffer* attachRenderBuffer(const SPK::Group &group) const override;
    void render(const SPK::Group &group, const SPK::DataSet *dataSet, SPK::RenderBuffer *renderBuffer) const override;
    void computeAABB(SPK::Vector3D &AABBMin, SPK::Vector3D &AABBMax, const SPK::Group &group, const SPK::DataSet *dataSet) const override;

    // Creates and registers a new QuadParticleSystemRenderer.
    static SPK::Ref<QuadParticleSystemRenderer> create(float scaleX = 1.0f, float scaleY = 1.0f)
    {
        return SPK_NEW(QuadParticleSystemRenderer, scaleX, scaleY);
    }
};

}
