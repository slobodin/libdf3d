#pragma once

#include <SPARK.h>

#include <libdf3d/render/RenderOperation.h>
#include <libdf3d/render/RenderCommon.h>
#include <libdf3d/render/Vertex.h>

namespace df3d {

class RenderQueue;
class Texture;
class RenderPass;
class VertexData;

namespace particlesys_impl {

class MyRenderBuffer;

// A cache for all systems vertex & index data.
class ParticleSystemBuffers_Quad
{
    const size_t INITIAL_CAPACITY = 64;
    const VertexFormat VERTEX_FORMAT = VertexFormat({ VertexFormat::POSITION_3, VertexFormat::TX_2, VertexFormat::COLOR_4 });

    size_t m_particlesAllocated = 0;

    unique_ptr<VertexBuffer> m_vb;
    unique_ptr<IndexBuffer> m_ib;

    // CPU side storage, TODO: use glMapBuffer
    unique_ptr<VertexData> m_vertexData;

    size_t m_currentVertexIndex = 0;
    size_t m_currentColorIndex = 0;
    size_t m_currentTexCoordIndex = 0;

public:
    ParticleSystemBuffers_Quad();
    ~ParticleSystemBuffers_Quad();

    void realloc(size_t nbParticles);

    void positionAtStart();
    void setNextVertex(const SPK::Vector3D &vertex);
    void setNextColor(const SPK::Color &color);
    void setNextVertexAndColor(const SPK::Vector3D &vertex, const SPK::Color &color);
    void setNextTexCoords(float u, float v);
    size_t getParticlesAllocated() const { return m_particlesAllocated; }

    void draw(size_t nbOfParticles, RenderPass *passProps, const glm::mat4 &m);
};

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

//! A Renderer drawing particles as quads.
class QuadParticleSystemRenderer : public ParticleSystemRenderer, public SPK::QuadRenderBehavior, public SPK::Oriented3DRenderBehavior
{
    spark_description(QuadParticleSystemRenderer, ParticleSystemRenderer)();
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

/*
// A renderer drawing particles as lines.
class LineParticleSystemRenderer : public ParticleSystemRenderer, public SPK::LineRenderBehavior
{
    LineParticleSystemRenderer(float length, float width);

public:
    ~LineParticleSystemRenderer();

    SPK::RenderBuffer* attachRenderBuffer(const SPK::Group &group) const override;
    void render(const SPK::Group &group, const SPK::DataSet *dataSet, SPK::RenderBuffer *renderBuffer) const override;
    void computeAABB(SPK::Vector3D &AABBMin, SPK::Vector3D &AABBMax, const SPK::Group &group, const SPK::DataSet *dataSet) const override;

    // Creates and registers a new LineParticleSystemRenderer.
    static SPK::Ref<LineParticleSystemRenderer> create(float length = 1.0f, float width = 1.0f)
    {
        return SPK_NEW(LineParticleSystemRenderer, length, width);
    }
};
*/

inline SPK::Vector3D glmToSpk(const glm::vec3 &v)
{
    return SPK::Vector3D(v.x, v.y, v.z);
}

inline glm::vec3 spkToGlm(const SPK::Vector3D &v)
{
    return glm::vec3(v.x, v.y, v.z);
}

} }
