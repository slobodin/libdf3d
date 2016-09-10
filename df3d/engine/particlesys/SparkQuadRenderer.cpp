#include "SparkQuadRenderer.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/render/RenderManager.h>
#include <df3d/engine/render/RenderPass.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/engine/render/RenderOperation.h>
#include <df3d/engine/render/Vertex.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/game/World.h>

namespace df3d { namespace particlesys_impl {

static const size_t QUAD_VERTICES_PER_PARTICLE = 4;
static const size_t QUAD_INDICES_PER_PARTICLE = 6;

class MyRenderBuffer : public SPK::RenderBuffer
{
public:
    ParticleSystemBuffers_Quad *m_buffers = nullptr;

    MyRenderBuffer(size_t nbParticles, ParticleSystemBuffers_Quad *buffers)
        : m_buffers(buffers)
    {
        if (nbParticles > m_buffers->getParticlesAllocated())
            m_buffers->realloc(nbParticles);
    }
};

void ParticleSystemBuffers_Quad::cleanup()
{
    if (m_vertexData)
    {
        MemoryManager::allocDefault()->dealloc(m_vertexData);
        m_vertexData = nullptr;
    }

    if (m_indexBuffer.isValid())
        svc().renderManager().getBackend().destroyIndexBuffer(m_indexBuffer);
}

ParticleSystemBuffers_Quad::ParticleSystemBuffers_Quad()
{

}

ParticleSystemBuffers_Quad::~ParticleSystemBuffers_Quad()
{
    cleanup();
}

void ParticleSystemBuffers_Quad::realloc(size_t nbParticles)
{
    cleanup();

    nbParticles = std::max(nbParticles, INITIAL_CAPACITY);
    size_t verticesCount = nbParticles * QUAD_VERTICES_PER_PARTICLE;

    DF3D_ASSERT_MESS(verticesCount < 0xFFFF, "Using 16-bit indices for particle system");

    // Allocate main memory storage copy (no glMapBuffer on ES2.0)
    m_vertexData = (Vertex_p3_tx2_c4*)MemoryManager::allocDefault()->alloc(sizeof(Vertex_p3_tx2_c4) * verticesCount);

    positionAtStart();

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

    // Initialize GPU storage of index array.
    m_indexBuffer = svc().renderManager().getBackend().createIndexBuffer(nbParticles * QUAD_INDICES_PER_PARTICLE,
                                                                         indexData.data(),
                                                                         GpuBufferUsageType::STATIC,
                                                                         INDICES_16_BIT);

    m_particlesAllocated = nbParticles;
}

void ParticleSystemBuffers_Quad::draw(size_t nbOfParticles, RenderPass *passProps, const glm::mat4 &m)
{
    if (UNLIKELY(nbOfParticles == 0))
        return;

    DF3D_ASSERT(nbOfParticles <= m_particlesAllocated);

    // Stream draw is more efficient than updating existent vertex buffer on mobile GPU.
    auto vb = svc().renderManager().getBackend().createVertexBuffer(Vertex_p3_tx2_c4::getFormat(),
                                                                    nbOfParticles * QUAD_VERTICES_PER_PARTICLE,
                                                                    m_vertexData,
                                                                    GpuBufferUsageType::STREAM);

    RenderOperation op;
    op.topology = Topology::TRIANGLES;
    op.indexBuffer = m_indexBuffer;
    op.vertexBuffer = vb;
    op.passProps = passProps;
    op.worldTransform = m;
    op.numberOfElements = nbOfParticles * QUAD_INDICES_PER_PARTICLE;

    svc().renderManager().drawRenderOperation(op);

    svc().renderManager().getBackend().destroyVertexBuffer(vb);
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
    renderBuffer.m_buffers->setNextVertex(particle.position() + quadSide() + quadUp());
    renderBuffer.m_buffers->setNextColor(color);
    // top left vertex
    renderBuffer.m_buffers->setNextVertex(particle.position() - quadSide() + quadUp());
    renderBuffer.m_buffers->setNextColor(color);
    // bottom left vertex
    renderBuffer.m_buffers->setNextVertex(particle.position() - quadSide() - quadUp());
    renderBuffer.m_buffers->setNextColor(color);
    // bottom right vertex
    renderBuffer.m_buffers->setNextVertex(particle.position() + quadSide() - quadUp());
    renderBuffer.m_buffers->setNextColor(color);
}

void QuadParticleSystemRenderer::fillBufferTexture2DCoordsAtlas(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const
{
    computeAtlasCoordinates(particle);

    // FIXME: inverted UV's Y because of OpenGL.
    renderBuffer.m_buffers->setNextTexCoords(textureAtlasU1(), textureAtlasV0());
    renderBuffer.m_buffers->setNextTexCoords(textureAtlasU0(), textureAtlasV0());
    renderBuffer.m_buffers->setNextTexCoords(textureAtlasU0(), textureAtlasV1());
    renderBuffer.m_buffers->setNextTexCoords(textureAtlasU1(), textureAtlasV1());
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
    return SPK_NEW(MyRenderBuffer, group.getCapacity(), m_quadBuffers);
}

void QuadParticleSystemRenderer::render(const SPK::Group &group, const SPK::DataSet *dataSet, SPK::RenderBuffer *renderBuffer) const
{
    if (!isActive())
        return;

    auto &buffer = static_cast<MyRenderBuffer&>(*renderBuffer);
    buffer.m_buffers->positionAtStart(); // Repositions all the buffers at the start.

    m_pass->enableDepthWrite(isRenderingOptionEnabled(SPK::RENDERING_OPTION_DEPTH_WRITE));

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
                buffer.m_buffers->setNextTexCoords(1.0f, 0.0f);
                buffer.m_buffers->setNextTexCoords(0.0f, 0.0f);
                buffer.m_buffers->setNextTexCoords(0.0f, 1.0f);
                buffer.m_buffers->setNextTexCoords(1.0f, 1.0f);
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

    buffer.m_buffers->draw(group.getNbParticles(), m_pass.get(), *m_currentTransformation);
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

} }
