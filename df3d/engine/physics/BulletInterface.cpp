#include "BulletInterface.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/render/RenderManager.h>
#include <df3d/engine/render/Material.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/engine/render/RenderQueue.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/TextureResource.h>
#include "PhysicsHelpers.h"

namespace df3d {

static unique_ptr<RenderPass> CreateDebugDrawPass()
{
    auto &embedResources = svc().renderManager().getEmbedResources();
    auto pass = make_unique<RenderPass>();
    pass->faceCullMode = FaceCullMode::NONE;
    pass->blendMode = BlendingMode::ALPHA;
    pass->isTransparent = true;
    pass->setParam("material_diffuse", glm::vec4(1.0f, 1.0f, 1.0f, 0.7f));
    pass->setParam("diffuseMap", embedResources.whiteTexture);
    pass->program = embedResources.coloredProgram;

    return pass;
}

BulletDebugDraw::BulletDebugDraw()
    : m_pass(CreateDebugDrawPass()),
    m_vertexData(Vertex_p_tx_c::getFormat())
{
    m_vertexData.addVertices(MAX_VERTICES);
}

BulletDebugDraw::~BulletDebugDraw()
{
    clean();
}

void BulletDebugDraw::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
    if (m_currentVertex >= MAX_VERTICES)
    {
        DFLOG_WARN("Bullet debug draw: vertices limit");
        return;
    }

    // FIXME: map directly to GPU.
    auto v1 = (Vertex_p_tx_c*)m_vertexData.getVertex(m_currentVertex++);
    v1->color = { color.x(), color.y(), color.z(), 1.0f };
    v1->pos = PhysicsHelpers::btToGlm(from);

    auto v2 = (Vertex_p_tx_c*)m_vertexData.getVertex(m_currentVertex++);
    v2->color = { color.x(), color.y(), color.z(), 1.0f };
    v2->pos = PhysicsHelpers::btToGlm(to);
}

void BulletDebugDraw::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
{

}

void BulletDebugDraw::reportErrorWarning(const char *warningString)
{
    DFLOG_WARN("Bullet physics warning: %s", warningString);
}

void BulletDebugDraw::draw3dText(const btVector3 &location, const char *textString)
{

}

void BulletDebugDraw::setDebugMode(int debugMode)
{

}

int BulletDebugDraw::getDebugMode() const
{
    return btIDebugDraw::DBG_DrawAabb | btIDebugDraw::DBG_DrawWireframe;// | btIDebugDraw::DBG_DrawNormals | btIDebugDraw::DBG_DrawConstraints;
}

void BulletDebugDraw::clean()
{
    if (m_vertexBuffer.isValid())
    {
        svc().renderManager().getBackend().destroyVertexBuffer(m_vertexBuffer);
        m_vertexBuffer = {};
    }
    m_currentVertex = 0;
}

void BulletDebugDraw::flushRenderOperations(RenderQueue *ops)
{
    if (m_currentVertex == 0)
        return;

    DF3D_ASSERT_MESS(!m_vertexBuffer.isValid(), "bullet debug draw: invalid vertex buffer");

    m_vertexBuffer = svc().renderManager().getBackend().createVertexBuffer(
        m_vertexData.getFormat(), m_currentVertex, m_vertexData.getRawData(), GpuBufferUsageType::STREAM);

    RenderOperation op;
    op.passProps = m_pass.get();
    op.vertexBuffer = m_vertexBuffer;
    op.numberOfElements = m_currentVertex;
    op.topology = Topology::LINES;

    ops->debugDrawOperations.push_back(op);
}

}
