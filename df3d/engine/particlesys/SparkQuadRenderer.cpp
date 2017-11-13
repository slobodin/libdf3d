#include "SparkQuadRenderer.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/render/RenderManager.h>
#include <df3d/engine/render/Material.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/engine/render/RenderOperation.h>
#include <df3d/engine/render/Vertex.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/TextureResource.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/game/World.h>

namespace df3d {

namespace {
    const size_t QUAD_VERTICES_PER_PARTICLE = 4;
    const size_t QUAD_INDICES_PER_PARTICLE = 6;
    const size_t INITIAL_BUFFER_CAPACITY = 64;
}

class MyRenderBuffer : public SPK::RenderBuffer
{
    Vertex_p_tx_c *m_vertexData = nullptr;
    VertexBufferHandle m_vertexBuffer;

    size_t m_currentVertexIndex = 0;
    size_t m_currentColorIndex = 0;
    size_t m_currentTexCoordIndex = 0;

    size_t m_particlesAllocated = 0;

public:
    MyRenderBuffer(size_t nbParticles)
    {
        size_t verticesCount = nbParticles * QUAD_VERTICES_PER_PARTICLE;

        DF3D_ASSERT_MESS(verticesCount < 0xFFFF, "Using 16-bit indices for particle system");

        m_vertexData = MEMORY_ALLOC(MemoryManager::allocDefault(), Vertex_p_tx_c, verticesCount);
        m_vertexBuffer = svc().renderManager().getBackend().createDynamicVertexBuffer(Vertex_p_tx_c::getFormat(), verticesCount, nullptr);

        m_particlesAllocated = nbParticles;
    }

    ~MyRenderBuffer()
    {
        MEMORY_FREE(MemoryManager::allocDefault(), m_vertexData);
        svc().renderManager().getBackend().destroyVertexBuffer(m_vertexBuffer);
    }

    void positionAtStart()
    {
        // Repositions all the buffer pointers at the start.
        m_currentVertexIndex = 0;
        m_currentColorIndex = 0;
        m_currentTexCoordIndex = 0;
    }

    void setNextVertex(const SPK::Vector3D &vertex)
    {
        auto &v = m_vertexData[m_currentVertexIndex].pos;
        v.x = vertex.x;
        v.y = vertex.y;
        v.z = vertex.z;
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
        auto &uv = m_vertexData[m_currentTexCoordIndex].uv;
        uv.x = u;
        uv.y = v;
        ++m_currentTexCoordIndex;
    }

    void draw(size_t nbOfParticles, RenderPass *passProps, const glm::mat4 &m, ParticleSystemIndexBuffer *indexBuffer)
    {
        if (nbOfParticles == 0)
            return;

        DF3D_ASSERT(nbOfParticles <= m_particlesAllocated);

        auto verticesCount = nbOfParticles * QUAD_VERTICES_PER_PARTICLE;
        svc().renderManager().getBackend().updateVertexBuffer(m_vertexBuffer, 0, verticesCount, m_vertexData);

        RenderOperation op;
        op.topology = Topology::TRIANGLES;
        op.indexBuffer = indexBuffer->getHandle();
        op.vertexBuffer = m_vertexBuffer;
        op.passProps = passProps;
        op.worldTransform = m;
        op.numberOfElements = nbOfParticles * QUAD_INDICES_PER_PARTICLE;

        svc().renderManager().drawRenderOperation(op);
    }
};

void ParticleSystemIndexBuffer::cleanup()
{
    if (m_indexBuffer.isValid())
        svc().renderManager().getBackend().destroyIndexBuffer(m_indexBuffer);
    m_indexBuffer = {};
}

ParticleSystemIndexBuffer::ParticleSystemIndexBuffer()
{
    reallocIfNeeded(INITIAL_BUFFER_CAPACITY);
}

ParticleSystemIndexBuffer::~ParticleSystemIndexBuffer()
{
    cleanup();
}

void ParticleSystemIndexBuffer::reallocIfNeeded(size_t nbParticles)
{
    if (nbParticles <= m_particlesAllocated)
        return;

    cleanup();

    nbParticles = std::max(nbParticles, INITIAL_BUFFER_CAPACITY);

    // Initialize the index array.
    PodArray<uint16_t> indexData(MemoryManager::allocDefault());
    indexData.resize(nbParticles * QUAD_INDICES_PER_PARTICLE);

    uint16_t currentIndex = 0;
    for (uint16_t i = 0; i < nbParticles; ++i)
    {
        indexData[currentIndex++] = QUAD_VERTICES_PER_PARTICLE * i + 0;
        indexData[currentIndex++] = QUAD_VERTICES_PER_PARTICLE * i + 1;
        indexData[currentIndex++] = QUAD_VERTICES_PER_PARTICLE * i + 2;
        indexData[currentIndex++] = QUAD_VERTICES_PER_PARTICLE * i + 0;
        indexData[currentIndex++] = QUAD_VERTICES_PER_PARTICLE * i + 2;
        indexData[currentIndex++] = QUAD_VERTICES_PER_PARTICLE * i + 3;
    }

    // Initialize GPU storage for index array.
    m_indexBuffer = svc().renderManager().getBackend().createIndexBuffer(indexData.size(), indexData.data(), INDICES_16_BIT);

    m_particlesAllocated = nbParticles;
}

void QuadParticleSystemRenderer::render2D(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const
{
    scaleQuadVectors(particle, scaleX, scaleY);
    fillBufferColorAndVertex(particle, renderBuffer);
}

void QuadParticleSystemRenderer::render2DRot(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const
{
    rotateAndScaleQuadVectors(particle, scaleX, scaleY);
    fillBufferColorAndVertex(particle, renderBuffer);
}

void QuadParticleSystemRenderer::render2DAtlas(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const
{
    scaleQuadVectors(particle, scaleX, scaleY);
    fillBufferColorAndVertex(particle, renderBuffer);
    fillBufferTexture2DCoordsAtlas(particle, renderBuffer);
}

void QuadParticleSystemRenderer::render2DAtlasRot(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const
{
    rotateAndScaleQuadVectors(particle, scaleX, scaleY);
    fillBufferColorAndVertex(particle, renderBuffer);
    fillBufferTexture2DCoordsAtlas(particle, renderBuffer);
}

void QuadParticleSystemRenderer::fillBufferColorAndVertex(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const
{
    const auto &color = particle.getColor();

    // Quads are drawn in a counter clockwise order.

    // top right vertex
    renderBuffer.setNextVertex(particle.position() + quadSide() + quadUp());
    renderBuffer.setNextColor(color);
    // top left vertex
    renderBuffer.setNextVertex(particle.position() - quadSide() + quadUp());
    renderBuffer.setNextColor(color);
    // bottom left vertex
    renderBuffer.setNextVertex(particle.position() - quadSide() - quadUp());
    renderBuffer.setNextColor(color);
    // bottom right vertex
    renderBuffer.setNextVertex(particle.position() + quadSide() - quadUp());
    renderBuffer.setNextColor(color);
}

void QuadParticleSystemRenderer::fillBufferTexture2DCoordsAtlas(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const
{
    computeAtlasCoordinates(particle);

    // FIXME: inverted UV's Y because of OpenGL.
    renderBuffer.setNextTexCoords(textureAtlasU1(), textureAtlasV0());
    renderBuffer.setNextTexCoords(textureAtlasU0(), textureAtlasV0());
    renderBuffer.setNextTexCoords(textureAtlasU0(), textureAtlasV1());
    renderBuffer.setNextTexCoords(textureAtlasU1(), textureAtlasV1());
}

QuadParticleSystemRenderer::QuadParticleSystemRenderer(float scaleX, float scaleY)
    : ParticleSystemRenderer(false),
    SPK::QuadRenderBehavior(scaleX, scaleY)
{

}

QuadParticleSystemRenderer::~QuadParticleSystemRenderer()
{

}

SPK::RenderBuffer* QuadParticleSystemRenderer::attachRenderBuffer(const SPK::Group &group) const
{
    auto buffer = SPK_NEW(MyRenderBuffer, group.getCapacity());
    m_indexBuffer->reallocIfNeeded(group.getCapacity());
    return buffer;
}

void QuadParticleSystemRenderer::render(const SPK::Group &group, const SPK::DataSet *dataSet, SPK::RenderBuffer *renderBuffer) const
{
    if (!isActive())
        return;

    auto &buffer = static_cast<MyRenderBuffer&>(*renderBuffer);
    buffer.positionAtStart(); // Repositions all the buffers at the start.

    m_pass.setDepthWrite(isRenderingOptionEnabled(SPK::RENDERING_OPTION_DEPTH_WRITE));

    switch (texturingMode)
    {
    case SPK::TEXTURE_MODE_NONE:
        break;
    case SPK::TEXTURE_MODE_2D:
        if (!group.isEnabled(SPK::PARAM_TEXTURE_INDEX))
        {
            // FIXME: inverted UV's Y because of OpenGL.
            for (size_t i = 0; i < group.getNbParticles(); ++i)
            {
                buffer.setNextTexCoords(1.0f, 0.0f);
                buffer.setNextTexCoords(0.0f, 0.0f);
                buffer.setNextTexCoords(0.0f, 1.0f);
                buffer.setNextTexCoords(1.0f, 1.0f);
            }
        }
        break;
    case SPK::TEXTURE_MODE_3D:
        DFLOG_WARN("3D texture for particle systems is not implemented");
        return;
    default:
        break;
    }

    if (group.isEnabled(SPK::PARAM_TEXTURE_INDEX))
    {
        if (group.isEnabled(SPK::PARAM_ANGLE))
            m_renderParticle = &QuadParticleSystemRenderer::render2DAtlasRot;
        else
            m_renderParticle = &QuadParticleSystemRenderer::render2DAtlas;
    }
    else
    {
        if (group.isEnabled(SPK::PARAM_ANGLE))
            m_renderParticle = &QuadParticleSystemRenderer::render2DRot;
        else
            m_renderParticle = &QuadParticleSystemRenderer::render2D;
    }

    auto camMatr = svc().defaultWorld().getCamera()->getViewMatrix() * *m_currentTransformation;
    camMatr = glm::inverse(camMatr);

    bool globalOrientation = precomputeOrientation3D(group,
    { -camMatr[2][0], -camMatr[2][1], -camMatr[2][2] },
    { camMatr[1][0], camMatr[1][1], camMatr[1][2] },
    { camMatr[3][0], camMatr[3][1], camMatr[3][2] });

    if (globalOrientation)
    {
        computeGlobalOrientation3D(group);

        for (SPK::ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
            (this->*m_renderParticle)(*particleIt, buffer);
    }
    else
    {
        for (SPK::ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
        {
            computeSingleOrientation3D(*particleIt);
            (this->*m_renderParticle)(*particleIt, buffer);
        }
    }

    buffer.draw(group.getNbParticles(), &m_pass, *m_currentTransformation, m_indexBuffer);
}

void QuadParticleSystemRenderer::computeAABB(SPK::Vector3D &AABBMin, SPK::Vector3D &AABBMax, const SPK::Group &group, const SPK::DataSet *dataSet) const
{
    float diagonal = group.getGraphicalRadius() * std::sqrt(scaleX * scaleX + scaleY * scaleY);
    SPK::Vector3D diagV(diagonal, diagonal, diagonal);

    if (group.isEnabled(SPK::PARAM_SCALE))
    {
        for (SPK::ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
        {
            SPK::Vector3D scaledDiagV = diagV * particleIt->getParamNC(SPK::PARAM_SCALE);
            AABBMin.setMin(particleIt->position() - scaledDiagV);
            AABBMax.setMax(particleIt->position() + scaledDiagV);
        }
    }
    else
    {
        for (SPK::ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
        {
            AABBMin.setMin(particleIt->position());
            AABBMax.setMax(particleIt->position());
        }
        AABBMin -= diagV;
        AABBMax += diagV;
    }
}

}
