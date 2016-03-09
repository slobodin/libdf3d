#pragma once

#include "SparkCommon.h"

#include <libdf3d/render/Vertex.h>

namespace df3d {

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

#pragma pack(push, 1)
    struct SpkVertexData
    {
        SPK::Vector3D pos;
        float tx_u;
        float tx_v;
        glm::vec4 color;
    };
#pragma pack(pop)

    // CPU side storage, TODO: use glMapBuffer
    SpkVertexData *m_vertexData = nullptr;

    size_t m_currentVertexIndex = 0;
    size_t m_currentColorIndex = 0;
    size_t m_currentTexCoordIndex = 0;

public:
    ParticleSystemBuffers_Quad();
    ~ParticleSystemBuffers_Quad();

    void realloc(size_t nbParticles);

    inline void positionAtStart()
    {
        // Repositions all the buffer pointers at the start.
        m_currentVertexIndex = 0;
        m_currentColorIndex = 0;
        m_currentTexCoordIndex = 0;
    }

    inline void setNextVertex(const SPK::Vector3D &vertex)
    {
        m_vertexData[m_currentVertexIndex].pos = vertex;
        ++m_currentVertexIndex;
    }

    void setNextColor(const SPK::Color &color)
    {
        auto &c = m_vertexData[m_currentColorIndex].color;

        c.r = color.r / 255.0f;
        c.g = color.g / 255.0f;
        c.b = color.b / 255.0f;
        c.a = color.a / 255.0f;

        ++m_currentColorIndex;
    }

    void setNextTexCoords(float u, float v)
    {
        m_vertexData[m_currentTexCoordIndex].tx_u = u;
        m_vertexData[m_currentTexCoordIndex].tx_v = v;

        ++m_currentTexCoordIndex;
    }

    size_t getParticlesAllocated() const { return m_particlesAllocated; }

    void draw(size_t nbOfParticles, RenderPass *passProps, const glm::mat4 &m);
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

} }
