#include "BulletInterface.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/render/RenderManager.h>
#include <df3d/engine/render/RenderPass.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/engine/render/RenderQueue.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceFactory.h>

namespace df3d { namespace physics_impl {

static unique_ptr<RenderPass> CreateDebugDrawPass()
{
    auto pass = make_unique<RenderPass>("bullet_debug_draw_pass");
    pass->setFaceCullMode(FaceCullMode::NONE);
    pass->setBlendMode(BlendingMode::ALPHA);
    pass->getPassParam("material_diffuse")->setValue(glm::vec4(1.0f, 1.0f, 1.0f, 0.7f));
    // FIXME: force to use default white texture because using colored shader.
    pass->getPassParam("diffuseMap")->setValue(nullptr);

    auto program = svc().resourceManager().getFactory().createColoredGpuProgram();
    pass->setGpuProgram(program);

    return pass;
}

BulletDebugDraw::BulletDebugDraw()
{

}

BulletDebugDraw::~BulletDebugDraw()
{
    clean();
}

void BulletDebugDraw::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{

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

}

void BulletDebugDraw::flushRenderOperations(RenderQueue *ops)
{

}

} }
