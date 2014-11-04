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

namespace df3d { namespace particlesys {

const std::string SPARK_BUFFER_NAME = "ps_buffer";

class MyBuffer : public SPK::Buffer
{
public:
    shared_ptr<render::VertexBuffer> m_buffer;
    shared_ptr<render::IndexBuffer> m_indexBuffer;

    void swap(size_t index0, size_t index1)
    {
        // Not implemented.
        assert(false);
    }
};

struct MyBufferCreator : public SPK::BufferCreator
{
    // Cache this value, because we always have "p:3, tx:2, c:4" vertex format.
    static const size_t STRIDE_BETWEEN_VERTICES;
    static const std::string PS_VERTEX_FORMAT;

    SPK::Buffer *createBuffer(size_t nbParticles, const SPK::Group& group) const
    {
        MyBuffer *retRes = new MyBuffer();

        auto vb = make_shared<render::VertexBuffer>(render::VertexFormat::create(PS_VERTEX_FORMAT));
        auto ib = make_shared<render::IndexBuffer>();
        vb->setUsageType(render::GB_USAGE_STREAM);
        ib->setUsageType(render::GB_USAGE_STATIC);

        // 4 vertices per quad.
        vb->resize(nbParticles * 4);
        // 6 indices per quad.
        ib->getIndices().resize(nbParticles * 6);

        retRes->m_buffer = vb;
        retRes->m_indexBuffer = ib;

        return retRes;
    }
};

const std::string MyBufferCreator::PS_VERTEX_FORMAT = "p:3, tx:2, c:4";
const size_t MyBufferCreator::STRIDE_BETWEEN_VERTICES = 9;

ParticleSystemRenderer::ParticleSystemRenderer()
{
    m_pass = make_shared<render::RenderPass>();
    m_pass->setFaceCullMode(render::RenderPass::FCM_NONE);
    m_pass->setFrontFaceWinding(render::RenderPass::WO_CCW);
    m_pass->setDiffuseColor(1.0f, 1.0f, 1.0f);

    auto program = g_resourceManager->getResource<render::GpuProgram>(render::COLORED_PROGRAM_EMBED_PATH);
    m_pass->setGpuProgram(program);
}

ParticleSystemRenderer::~ParticleSystemRenderer()
{
}

void ParticleSystemRenderer::setBlending(SPK::BlendingMode blendMode)
{
    switch (blendMode)
    {
    case SPK::BLENDING_NONE:
        m_pass->setBlendMode(render::RenderPass::BM_NONE);
        break;
    case SPK::BLENDING_ADD:
        m_pass->setBlendMode(render::RenderPass::BM_ADDALPHA);
        break;
    case SPK::BLENDING_ALPHA:
        m_pass->setBlendMode(render::RenderPass::BM_ALPHA);
        break;
    default:
        break;
    }
}

void ParticleSystemRenderer::setDiffuseMap(shared_ptr<render::Texture> texture)
{
    m_pass->setSampler("diffuseMap", texture);
}

shared_ptr<render::VertexBuffer> ParticleSystemRenderer::getVertexBuffer() const
{
    return m_currentBuffer->m_buffer;
}

shared_ptr<render::IndexBuffer> ParticleSystemRenderer::getIndexBuffer() const
{
    return m_currentBuffer->m_indexBuffer;
}

bool QuadParticleSystemRenderer::checkBuffers(const SPK::Group& group)
{
    m_currentBuffer = dynamic_cast<MyBuffer *>(group.getBuffer(SPARK_BUFFER_NAME, 0));
    return m_currentBuffer != nullptr;
}

void QuadParticleSystemRenderer::render2D(const SPK::Particle& particle) const
{
    // FIXME:
    // May be map gpu data?

    scaleQuadVectors(particle, scaleX, scaleY);

    // Each particle has 4 vertices.
    size_t idx = particle.getIndex() << 2;
    auto vb = m_currentBuffer->m_buffer->getVertexData();

    float r = particle.getR();
    float g = particle.getG();
    float b = particle.getB();

    const auto &position = particle.position();
    const auto &right = quadSide();
    const auto &up = quadUp();

    float *iterator = vb + idx * MyBufferCreator::STRIDE_BETWEEN_VERTICES;

    // Update left-top vertex.
    *(iterator++) = position.x - right.x + up.x;
    *(iterator++) = position.y - right.y + up.y;
    *(iterator++) = position.z - right.z + up.z;

    // Skip texture coordinates.
    iterator += 2;
    iterator[0] = r; iterator[1] = g; iterator[2] = b; iterator[3] = 1.0f;

    // Skip this vertex color.
    iterator += 4;

    // Update left-bottom vertex.
    *(iterator++) = position.x - right.x - up.x;
    *(iterator++) = position.y - right.y - up.y;
    *(iterator++) = position.z - right.z - up.z;

    // Skip texture coordinates.
    iterator += 2;
    iterator[0] = r; iterator[1] = g; iterator[2] = b; iterator[3] = 1.0f;

    // Skip this vertex color.
    iterator += 4;

    // Update right-bottom vertex.
    *(iterator++) = position.x + right.x - up.x;
    *(iterator++) = position.y + right.y - up.y;
    *(iterator++) = position.z + right.z - up.z;

    // Skip texture coordinates.
    iterator += 2;
    iterator[0] = r; iterator[1] = g; iterator[2] = b; iterator[3] = 1.0f;

    // Skip this vertex color.
    iterator += 4;

    // Update right-top vertex.
    *(iterator++) = position.x + right.x + up.x;
    *(iterator++) = position.y + right.y + up.y;
    *(iterator++) = position.z + right.z + up.z;

    // Skip texture coordinates.
    iterator += 2;
    iterator[0] = r; iterator[1] = g; iterator[2] = b; iterator[3] = 1.0f;
}

void QuadParticleSystemRenderer::render2DRot(const SPK::Particle& particle) const
{
    rotateAndScaleQuadVectors(particle, scaleX, scaleY);
}

QuadParticleSystemRenderer::QuadParticleSystemRenderer(float scaleX, float scaleY)
    : QuadRendererInterface(scaleX, scaleY)
{

}

QuadParticleSystemRenderer::~QuadParticleSystemRenderer()
{

}

void QuadParticleSystemRenderer::createBuffers(const SPK::Group& group)
{
    m_currentBuffer = dynamic_cast<MyBuffer *>(group.createBuffer(SPARK_BUFFER_NAME, MyBufferCreator(), 0, false));

    size_t totalParticles = group.getParticles().getNbReserved();
    auto indices = m_currentBuffer->m_indexBuffer->getRawIndices();
    auto vertices = m_currentBuffer->m_buffer->getVertexData();
    auto stride = MyBufferCreator::STRIDE_BETWEEN_VERTICES;

    size_t base = 0;
    size_t idxBase = 0;

    for (size_t i = 0; i < totalParticles; i++)
    {
        // Filling index buffer.
        indices[idxBase + 0] = base + 0;
        indices[idxBase + 1] = base + 1;
        indices[idxBase + 2] = base + 2;
        indices[idxBase + 3] = base + 0;
        indices[idxBase + 4] = base + 2;
        indices[idxBase + 5] = base + 3;

        render::Vertex_3p2tx4c *v = nullptr;

        // Filling tx coordinates.
        // CCW order.
        v = reinterpret_cast<render::Vertex_3p2tx4c *>(vertices + (base + 0) * stride);
        v->tx = glm::vec2(0.0f, 1.0f);

        v = reinterpret_cast<render::Vertex_3p2tx4c *>(vertices + (base + 1) * stride);
        v->tx = glm::vec2(0.0f, 0.0f);

        v = reinterpret_cast<render::Vertex_3p2tx4c *>(vertices + (base + 2) * stride);
        v->tx = glm::vec2(1.0f, 0.0f);

        v = reinterpret_cast<render::Vertex_3p2tx4c *>(vertices + (base + 3) * stride);
        v->tx = glm::vec2(1.0f, 1.0f);

        // 4 verts per quad.
        base += 4;
        // 6 ind per quad.
        idxBase += 6;
    }
}

void QuadParticleSystemRenderer::destroyBuffers(const SPK::Group& group)
{
    group.destroyBuffer(SPARK_BUFFER_NAME);
}

void QuadParticleSystemRenderer::render(const SPK::Group& group)
{
    if (!prepareBuffers(group))
        return;

    m_pass->enableDepthTest(isRenderingHintEnabled(SPK::DEPTH_TEST));
    m_pass->enableDepthWrite(isRenderingHintEnabled(SPK::DEPTH_WRITE));

    switch (texturingMode)
    {
    case SPK::TEXTURE_NONE:
        break;
    case SPK::TEXTURE_2D:
        break;
    case SPK::TEXTURE_3D:
        base::glog << "3D texture for particle systems is not implemented" << base::logwarn;
        return;
    default:
        break;
    }

    if (group.getModel()->isEnabled(SPK::PARAM_ANGLE))
    {
        m_renderParticle = &QuadParticleSystemRenderer::render2DRot;
    }
    else
    {
        m_renderParticle = &QuadParticleSystemRenderer::render2D;
    }

    const auto &dir = g_sceneManager->getCamera()->getDir();
    const auto &up = g_sceneManager->getCamera()->getUp();
    const auto &pos = g_sceneManager->getCamera()->transform()->getPosition();

    bool globalOrientation = precomputeOrientation3D(group, SPK::Vector3D(dir.x, dir.y, dir.z), SPK::Vector3D(up.x, up.y, up.z), SPK::Vector3D(pos.x, pos.y, pos.z));
    auto totalParticles = group.getNbParticles();

    if (globalOrientation)
    {
        computeGlobalOrientation3D();

        for (size_t i = 0; i < totalParticles; ++i)
            (this->*m_renderParticle)(group.getParticle(i));
    }
    else
    {
        for (size_t i = 0; i < totalParticles; ++i)
        {
            const SPK::Particle& particle = group.getParticle(i);
            computeSingleOrientation3D(particle);
            (this->*m_renderParticle)(particle);
        }
    }

    // 4 vertices and 6 indices per particle quad.
    m_currentBuffer->m_buffer->setElementsUsed(totalParticles * 4);
    m_currentBuffer->m_indexBuffer->setElementsUsed(totalParticles * 6);
    // Refill gpu with new data.
    m_currentBuffer->m_buffer->setDirty();
}

QuadParticleSystemRenderer* QuadParticleSystemRenderer::create(float scaleX, float scaleY)
{
    QuadParticleSystemRenderer* obj = new QuadParticleSystemRenderer(scaleX, scaleY);
    registerObject(obj);
    return obj;
}

void initSparkEngine()
{
    // Clamp the step to 100 ms.
    SPK::System::setClampStep(true, 0.1f);
    SPK::System::useRealStep();
}

void destroySparkEngine()
{
    SPK::SPKFactory::getInstance().traceAll();
}


} }