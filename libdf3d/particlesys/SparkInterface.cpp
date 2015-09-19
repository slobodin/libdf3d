#include "SparkInterface.h"

#include <render/RenderPass.h>
#include <base/Service.h>
#include <scene/Camera.h>
#include <components/TransformComponent.h>
#include <render/VertexIndexBuffer.h>
#include <render/GpuProgram.h>
#include <render/RenderOperation.h>
#include <render/RenderQueue.h>

namespace df3d { namespace particlesys {

const int QUAD_VERTICES_PER_PARTICLE = 4;
const int QUAD_INDICES_PER_PARTICLE = 6;
const int LINE_VERTICES_PER_PARTICLE = 2;
const int LINE_INDICES_PER_PARTICLE = 2;

class MyRenderBuffer : public SPK::RenderBuffer
{
    size_t m_currentIndexIndex;
    size_t m_currentVertexIndex;
    size_t m_currentColorIndex;
    size_t m_currentTexCoordIndex;

public:
    shared_ptr<render::VertexBuffer> m_vb;
    shared_ptr<render::IndexBuffer> m_ib;

    render::VertexData m_vertexData;
    render::IndexArray m_indexData;

    MyRenderBuffer(size_t nbParticles, int verticesPerParticle, int indicesPerParticle)
        : m_vertexData(render::VertexFormat({ render::VertexFormat::POSITION_3, render::VertexFormat::TX_2, render::VertexFormat::COLOR_4 }))
    {
        m_vb = make_shared<render::VertexBuffer>(m_vertexData.getFormat());
        m_ib = make_shared<render::IndexBuffer>();

        // Allocate GPU storage.
        m_vb->alloc(nbParticles * verticesPerParticle, nullptr, render::GpuBufferUsageType::DYNAMIC);
        m_ib->alloc(nbParticles * indicesPerParticle, nullptr, render::GpuBufferUsageType::STATIC);

        // Allocate main memory storage copy (no glMapBuffer on ES2.0)
        m_vertexData.alloc(nbParticles * verticesPerParticle);
        m_indexData.resize(nbParticles * indicesPerParticle);

        positionAtStart();
    }

    void positionAtStart()
    {
        // Repositions all the buffer pointers at the start.
        m_currentIndexIndex = 0;
        m_currentVertexIndex = 0;
        m_currentColorIndex = 0;
        m_currentTexCoordIndex = 0;
    }

    void setNextIndex(int index)
    {
        m_indexData[m_currentIndexIndex++] = index;
    }

    void setNextVertex(const SPK::Vector3D &vertex)
    {
        auto vert = m_vertexData.getVertex(m_currentVertexIndex++);
        vert.setPosition({ vertex.x, vertex.y, vertex.z });
    }

    void setNextColor(const SPK::Color &color)
    {
        auto vert = m_vertexData.getVertex(m_currentColorIndex++);
        vert.setColor({ color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f });
    }

    void setNextTexCoords(float u, float v)
    {
        auto vert = m_vertexData.getVertex(m_currentTexCoordIndex++);
        vert.setTx({ u, v });
    }
};

void ParticleSystemRenderer::addToRenderQueue(MyRenderBuffer &buffer, size_t nbOfParticles, int verticesPerParticle, int indicesPerParticle, render::RenderOperation::Type type) const
{
    // Refill GPU with new data (only vertices was changed).
    buffer.m_vb->update(nbOfParticles * verticesPerParticle, buffer.m_vertexData.getRawData());

    buffer.m_vb->setVerticesUsed(nbOfParticles * verticesPerParticle);
    buffer.m_ib->setIndicesUsed(nbOfParticles * indicesPerParticle);

    render::RenderOperation op;
    op.type = type;
    op.indexData = buffer.m_ib;
    op.vertexData = buffer.m_vb;
    op.passProps = m_pass;
    op.worldTransform = *m_currentTransformation;

    if (op.passProps->isTransparent())
        m_currentRenderQueue->transparentOperations.push_back(op);
    else
        m_currentRenderQueue->notLitOpaqueOperations.push_back(op);
}

ParticleSystemRenderer::ParticleSystemRenderer(bool NEEDS_DATASET)
    : SPK::Renderer(NEEDS_DATASET),
    m_pass(make_shared<render::RenderPass>())
{
    m_pass->setFaceCullMode(render::RenderPass::FaceCullMode::BACK);
    m_pass->setFrontFaceWinding(render::RenderPass::WindingOrder::CCW);
    m_pass->setDiffuseColor(1.0f, 1.0f, 1.0f);

    m_pass->setGpuProgram(gsvc().resourceMgr.getFactory().createColoredGpuProgram());
}

ParticleSystemRenderer::~ParticleSystemRenderer()
{

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

void ParticleSystemRenderer::enableFaceCulling(bool enable)
{
    if (enable)
    {
        m_pass->setFaceCullMode(render::RenderPass::FaceCullMode::BACK);
        m_pass->setFrontFaceWinding(render::RenderPass::WindingOrder::CCW);
    }
    else
    {
        m_pass->setFaceCullMode(render::RenderPass::FaceCullMode::NONE);
    }
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
    // Quads are drawn in a counter clockwise order.
    renderBuffer.setNextVertex(particle.position() + quadSide() + quadUp());    // top right vertex
    renderBuffer.setNextVertex(particle.position() - quadSide() + quadUp());    // top left vertex
    renderBuffer.setNextVertex(particle.position() - quadSide() - quadUp());    // bottom left vertex
    renderBuffer.setNextVertex(particle.position() + quadSide() - quadUp());    // bottom right vertex

    const auto &color = particle.getColor();
    renderBuffer.setNextColor(color);
    renderBuffer.setNextColor(color);
    renderBuffer.setNextColor(color);
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
    size_t totalParticles = group.getCapacity();
    auto buffer = SPK_NEW(MyRenderBuffer, totalParticles, QUAD_VERTICES_PER_PARTICLE, QUAD_INDICES_PER_PARTICLE);
    buffer->positionAtStart();

    // Initialize the index array.
    for (size_t i = 0; i < totalParticles; ++i)
    {
        buffer->setNextIndex(QUAD_VERTICES_PER_PARTICLE * i + 0);
        buffer->setNextIndex(QUAD_VERTICES_PER_PARTICLE * i + 1);
        buffer->setNextIndex(QUAD_VERTICES_PER_PARTICLE * i + 2);
        buffer->setNextIndex(QUAD_VERTICES_PER_PARTICLE * i + 0);
        buffer->setNextIndex(QUAD_VERTICES_PER_PARTICLE * i + 2);
        buffer->setNextIndex(QUAD_VERTICES_PER_PARTICLE * i + 3);
    }

    // Initialize GPU storage of index array.
    buffer->m_ib->update(totalParticles * QUAD_INDICES_PER_PARTICLE, buffer->m_indexData.data());
    // Clear main storage indices copy.
    buffer->m_indexData.clear();

    // Initialize the texture array (CCW order).
    for (size_t i = 0; i < group.getCapacity(); ++i)
    {
        // FIXME: inverted UV's Y because of OpenGL.
        buffer->setNextTexCoords(1.0f, 0.0f);
        buffer->setNextTexCoords(0.0f, 0.0f);
        buffer->setNextTexCoords(0.0f, 1.0f);
        buffer->setNextTexCoords(1.0f, 1.0f);
    }

    return buffer;
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
        if (!group.isEnabled(SPK::PARAM_TEXTURE_INDEX))
        {
            // FIXME: inverted UV's Y because of OpenGL.
            for (size_t i = 0; i < group.getCapacity(); ++i)
            {
                buffer.setNextTexCoords(1.0f, 0.0f);
                buffer.setNextTexCoords(0.0f, 0.0f);
                buffer.setNextTexCoords(0.0f, 1.0f);
                buffer.setNextTexCoords(1.0f, 1.0f);
            }
        }
        break;
    case SPK::TEXTURE_MODE_3D:
        base::glog << "3D texture for particle systems is not implemented" << base::logwarn;
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

    auto camMatr = gsvc().sceneMgr.getCamera()->getViewMatrix() * *m_currentTransformation;
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

    addToRenderQueue(buffer, group.getNbParticles(), QUAD_VERTICES_PER_PARTICLE, QUAD_INDICES_PER_PARTICLE, render::RenderOperation::Type::TRIANGLES);
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

LineParticleSystemRenderer::LineParticleSystemRenderer(float length, float width)
    : ParticleSystemRenderer(false),
    SPK::LineRenderBehavior(length, width)
{

}

LineParticleSystemRenderer::~LineParticleSystemRenderer()
{

}

SPK::RenderBuffer* LineParticleSystemRenderer::attachRenderBuffer(const SPK::Group &group) const
{
    size_t totalParticles = group.getCapacity();
    auto buffer = SPK_NEW(MyRenderBuffer, totalParticles, LINE_VERTICES_PER_PARTICLE, LINE_INDICES_PER_PARTICLE);
    buffer->positionAtStart();

    // Initialize the index array.
    for (size_t i = 0; i < totalParticles * LINE_INDICES_PER_PARTICLE; ++i)
        buffer->setNextIndex(i);

    buffer->m_ib->update(totalParticles * LINE_INDICES_PER_PARTICLE, buffer->m_indexData.data());
    buffer->m_indexData.clear();

    return buffer;
}

void LineParticleSystemRenderer::render(const SPK::Group &group, const SPK::DataSet *dataSet, SPK::RenderBuffer *renderBuffer) const
{
    auto &buffer = static_cast<MyRenderBuffer&>(*renderBuffer);
    buffer.positionAtStart(); // Repositions all the buffers at the start.

    m_pass->enableDepthWrite(isRenderingOptionEnabled(SPK::RENDERING_OPTION_DEPTH_WRITE));

    for (SPK::ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
    {
        buffer.setNextVertex(particleIt->position());
        buffer.setNextVertex(particleIt->position() + particleIt->velocity() * length);

        const auto &color = particleIt->getColor();
        buffer.setNextColor(color);
        buffer.setNextColor(color);
    }

    addToRenderQueue(buffer, group.getNbParticles(), LINE_VERTICES_PER_PARTICLE, LINE_INDICES_PER_PARTICLE, render::RenderOperation::Type::LINES);
}

void LineParticleSystemRenderer::computeAABB(SPK::Vector3D &AABBMin, SPK::Vector3D &AABBMax, const SPK::Group &group, const SPK::DataSet *dataSet) const
{
    for (SPK::ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
    {
        auto v = particleIt->position() + particleIt->velocity() * length;
        AABBMin.setMin(particleIt->position());
        AABBMin.setMin(v);
        AABBMax.setMax(particleIt->position());
        AABBMax.setMax(v);
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
