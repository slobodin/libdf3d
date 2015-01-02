#include "df3d_pch.h"
#include "SparkInterface.h"

#include <render/RenderPass.h>
#include <base/Controller.h>
#include <scene/SceneManager.h>
#include <scene/Camera.h>
#include <components/TransformComponent.h>
#include <resources/ResourceManager.h>
#include <render/VertexIndexBuffer.h>
#include <render/GpuProgram.h>
#include <render/RenderOperation.h>
#include <render/RenderQueue.h>

namespace df3d { namespace particlesys {

const int VERTICES_PER_PARTICLE = 4;
const int INDICES_PER_PARTICLE = 6;
const int STRIDE_BETWEEN_VERTICES = 9;  // p:3, tx:2, c:4 vertex format

class MyRenderBuffer : public SPK::RenderBuffer
{
    size_t m_currentIndexIndex;
    size_t m_currentVertexIndex;
    size_t m_currentColorIndex;
    size_t m_currentTexCoordIndex;

public:
    shared_ptr<render::VertexBuffer> m_vb;
    shared_ptr<render::IndexBuffer> m_ib;

    MyRenderBuffer(size_t nbParticles)
    {
        m_vb = make_shared<render::VertexBuffer>(render::VertexFormat::create("p:3, tx:2, c:4"));
        m_ib = make_shared<render::IndexBuffer>();
        m_vb->setUsageType(render::GpuBufferUsageType::STREAM);
        m_ib->setUsageType(render::GpuBufferUsageType::STATIC);

        // 4 vertices per quad.
        m_vb->resize(nbParticles * VERTICES_PER_PARTICLE);
        // 6 indices per quad.
        m_ib->getIndices().resize(nbParticles * INDICES_PER_PARTICLE);
    }

    void positionAtStart()
    {
        m_currentIndexIndex = 0;
        m_currentVertexIndex = 0;
        m_currentColorIndex = 0;
        m_currentTexCoordIndex = 0;
    }

    void setNextIndex(int index)
    {
        m_ib->getIndices()[m_currentIndexIndex++] = index;
    }

    void setNextVertex(const SPK::Vector3D &vertex)
    {
        auto vert = reinterpret_cast<render::Vertex_3p2tx4c *>(m_vb->getVertexData() + (m_currentVertexIndex++) * STRIDE_BETWEEN_VERTICES);
        vert->p.x = vertex.x;
        vert->p.y = vertex.y;
        vert->p.z = vertex.z;
    }

    void setNextColor(const SPK::Color &color)
    {
        auto vert = reinterpret_cast<render::Vertex_3p2tx4c *>(m_vb->getVertexData() + (m_currentColorIndex++) * STRIDE_BETWEEN_VERTICES);
        vert->color.r = color.r / 255.0f;
        vert->color.g = color.g / 255.0f;
        vert->color.b = color.b / 255.0f;
        vert->color.a = color.a / 255.0f;
    }

    void skipNextColors(size_t nb)
    {
        m_currentColorIndex += nb;
    }

    void setNextTexCoords(float u, float v)
    {
        auto vert = reinterpret_cast<render::Vertex_3p2tx4c *>(m_vb->getVertexData() + (m_currentTexCoordIndex++) * STRIDE_BETWEEN_VERTICES);
        vert->tx.x = u;
        vert->tx.y = v;
    }

    void skipNextTexCoords(size_t nb)
    {
        m_currentTexCoordIndex += nb;
    }
};

ParticleSystemRenderer::ParticleSystemRenderer(bool NEEDS_DATASET)
    : SPK::Renderer(NEEDS_DATASET)
{
    m_pass = make_shared<render::RenderPass>();
    m_pass->setFaceCullMode(render::RenderPass::FaceCullMode::BACK);
    m_pass->setFrontFaceWinding(render::RenderPass::WindingOrder::CCW);
    m_pass->setDiffuseColor(1.0f, 1.0f, 1.0f);

    auto program = g_resourceManager->getResource<render::GpuProgram>(render::COLORED_PROGRAM_EMBED_PATH);
    m_pass->setGpuProgram(program);
}

ParticleSystemRenderer::~ParticleSystemRenderer()
{
}

SPK::RenderBuffer* ParticleSystemRenderer::attachRenderBuffer(const SPK::Group &group) const
{
    size_t totalParticles = group.getCapacity();
    auto buffer = SPK_NEW(MyRenderBuffer, totalParticles);
    buffer->positionAtStart();

    // Initialize the index array.
    for (size_t i = 0; i < group.getCapacity(); ++i)
    {
        buffer->setNextIndex(VERTICES_PER_PARTICLE * i + 0);
        buffer->setNextIndex(VERTICES_PER_PARTICLE * i + 1);
        buffer->setNextIndex(VERTICES_PER_PARTICLE * i + 2);
        buffer->setNextIndex(VERTICES_PER_PARTICLE * i + 0);
        buffer->setNextIndex(VERTICES_PER_PARTICLE * i + 2);
        buffer->setNextIndex(VERTICES_PER_PARTICLE * i + 3);
    }

    // Initialize the texture array (CCW order).
    for (size_t i = 0; i < group.getCapacity(); ++i)
    {
        buffer->setNextTexCoords(1.0f, 1.0f);
        buffer->setNextTexCoords(0.0f, 1.0f);
        buffer->setNextTexCoords(0.0f, 0.0f);
        buffer->setNextTexCoords(1.0f, 0.0f);
    }

    return buffer;
}

void ParticleSystemRenderer::setBlendMode(SPK::BlendMode blendMode)
{
    switch (blendMode)
    {
    case SPK::BLEND_MODE_NONE:
        m_pass->setBlendMode(render::RenderPass::BlendingMode::NONE);
        break;
    case SPK::BLEND_MODE_ADD:
        m_pass->setBlendMode(render::RenderPass::BlendingMode::ADDALPHA);
        break;
    case SPK::BLEND_MODE_ALPHA:
        m_pass->setBlendMode(render::RenderPass::BlendingMode::ALPHA);
        break;
    default:
        break;
    }
}

void ParticleSystemRenderer::setDiffuseMap(shared_ptr<render::Texture> texture)
{
    m_pass->setSampler("diffuseMap", texture);
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

void QuadParticleSystemRenderer::fillBufferColorAndVertex(const SPK::Particle &particle, MyRenderBuffer &renderBuffer) const
{
    // Quads are drawn in a counter clockwise order.
    renderBuffer.setNextVertex(particle.position() + quadSide() + quadUp());	// top right vertex
    renderBuffer.setNextVertex(particle.position() - quadSide() + quadUp());	// top left vertex
    renderBuffer.setNextVertex(particle.position() - quadSide() - quadUp());	// bottom left vertex
    renderBuffer.setNextVertex(particle.position() + quadSide() - quadUp());	// bottom right vertex

    const auto &color = particle.getColor();
    renderBuffer.setNextColor(color);
    renderBuffer.setNextColor(color);
    renderBuffer.setNextColor(color);
    renderBuffer.setNextColor(color);
}

QuadParticleSystemRenderer::QuadParticleSystemRenderer(float scaleX, float scaleY)
    : ParticleSystemRenderer(false),
    SPK::QuadRenderBehavior(scaleX, scaleY)
{

}

QuadParticleSystemRenderer::~QuadParticleSystemRenderer()
{

}

void QuadParticleSystemRenderer::render(const SPK::Group &group, const SPK::DataSet *dataSet, SPK::RenderBuffer *renderBuffer) const
{
    auto &buffer = static_cast<MyRenderBuffer&>(*renderBuffer);
    buffer.positionAtStart(); // Repositions all the buffers at the start.

    m_pass->enableDepthWrite(isRenderingOptionEnabled(SPK::RENDERING_OPTION_DEPTH_WRITE));

    switch (texturingMode)
    {
    case SPK::TEXTURE_MODE_NONE:
        break;
    case SPK::TEXTURE_MODE_2D:
        break;
    case SPK::TEXTURE_MODE_3D:
        base::glog << "3D texture for particle systems is not implemented" << base::logwarn;
        return;
    default:
        break;
    }

    if (group.isEnabled(SPK::PARAM_ANGLE))
    {
        m_renderParticle = &QuadParticleSystemRenderer::render2DRot;
    }
    else
    {
        m_renderParticle = &QuadParticleSystemRenderer::render2D;
    }

    const auto &pos = g_sceneManager->getCamera()->transform()->getPosition();
    const auto &dir = g_sceneManager->getCamera()->getDir();
    const auto &up = g_sceneManager->getCamera()->getUp();

    bool globalOrientation = precomputeOrientation3D(group, glmToSpk(-dir), glmToSpk(up), glmToSpk(pos));

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

    auto totalParticles = group.getNbParticles();

    // 4 vertices and 6 indices per particle quad.
    buffer.m_vb->setElementsUsed(totalParticles * VERTICES_PER_PARTICLE);
    buffer.m_ib->setElementsUsed(totalParticles * INDICES_PER_PARTICLE);
    // Refill gpu with new data (only vertices are changed).
    buffer.m_vb->setDirty();

    render::RenderOperation op;
    op.indexData = buffer.m_ib;
    op.vertexData = buffer.m_vb;
    op.passProps = m_pass;

    if (op.passProps->isTransparent())
        m_currentRenderQueue->transparentOperations.push_back(op);
    else
        m_currentRenderQueue->notLitOpaqueOperations.push_back(op);
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

void initSparkEngine()
{
    // Clamp the step to 100 ms.
    SPK::System::setClampStep(true, 0.1f);
    SPK::System::useRealStep();
}

void destroySparkEngine()
{
    SPK_DUMP_MEMORY
}


} }