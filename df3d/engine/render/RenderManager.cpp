#include "RenderManager.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/EngineCVars.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/MaterialResource.h>
#include <df3d/engine/resources/GpuProgramResource.h>
#include <df3d/engine/resources/TextureResource.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/game/World.h>
#include <df3d/engine/gui/GuiManager.h>
#include <df3d/engine/particlesys/ParticleSystemComponentProcessor.h>
#include "RenderOperation.h"
#include "Material.h"
#include "Viewport.h"
#include "RenderQueue.h"
#include "IRenderBackend.h"
#include "GpuProgramSharedState.h"

#include <tb_widgets.h>
#include <animation/tb_widget_animation.h>

namespace df3d {

RenderManagerEmbedResources::RenderManagerEmbedResources(RenderManager *render)
{
    auto &allocator = MemoryManager::allocDefault();

    // Create white texture.
    {
        const auto w = 8;
        const auto h = 8;
        const auto pf = PixelFormat::RGBA;

        PodArray<uint8_t> pixels(allocator);
        pixels.resize(w * h * 4, 255);

        TextureInfo info;
        info.width = w;
        info.height = h;
        info.numMips = 0;
        info.format = pf;

        whiteTexture = render->getBackend().createTexture2D(info,
                                                            TEXTURE_FILTERING_NEAREST | TEXTURE_WRAP_MODE_REPEAT,
                                                            pixels.data());
    }

    // Load GPU programs.
    {
        const std::string colored_vert =
#include "gl/embed_glsl/colored_vert.h"
            ;
        const std::string colored_frag =
#include "gl/embed_glsl/colored_frag.h"
            ;
        const std::string ambient_vert =
#include "gl/embed_glsl/ambient_vert.h"
            ;
        const std::string ambient_frag =
#include "gl/embed_glsl/ambient_frag.h"
            ;

        coloredProgram = GpuProgramFromData(colored_vert, colored_frag, allocator);
        ambientPassProgram = GpuProgramFromData(ambient_vert, ambient_frag, allocator);
    }

    ambientPass = MAKE_NEW(allocator, RenderPass)();
    ambientPass->program = ambientPassProgram;
}

RenderManagerEmbedResources::~RenderManagerEmbedResources()
{
    auto &allocator = MemoryManager::allocDefault();
    svc().renderManager().getBackend().destroyTexture(whiteTexture);
    svc().renderManager().getBackend().destroyGpuProgram(coloredProgram->handle);
    svc().renderManager().getBackend().destroyGpuProgram(ambientPassProgram->handle);
    MAKE_DELETE(allocator, coloredProgram);
    MAKE_DELETE(allocator, ambientPassProgram);
    MAKE_DELETE(allocator, ambientPass);
}

void RenderManager::onFrameBegin()
{
    m_blendModeOverriden = false;
    m_depthTestOverriden = false;
    m_depthWriteOverriden = false;

    m_sharedState->clear();
    m_renderBackend->frameBegin();

    m_renderBackend->enableDepthTest(true);
    m_renderBackend->enableDepthWrite(true);
    m_renderBackend->enableScissorTest(false);
    m_renderBackend->clearColorBuffer({ 0.0f, 0.0f, 0.0f, 1.0f });
    m_renderBackend->clearDepthBuffer();
    m_renderBackend->clearStencilBuffer();
    m_renderBackend->setBlendingMode(BlendingMode::NONE);
}

void RenderManager::onFrameEnd()
{
    m_renderBackend->frameEnd();
}

void RenderManager::doRenderWorld(World &world)
{
    // TODO_ecs: what if render queue becomes big???
    m_renderQueue->clear();

    world.collectRenderOperations(m_renderQueue.get());

    m_renderBackend->setViewport(m_viewport.x(), m_viewport.y(), m_viewport.width(), m_viewport.height());
    m_sharedState->setViewPort(m_viewport);
    m_sharedState->setProjectionMatrix(world.getCamera()->getProjectionMatrix());
    m_sharedState->setViewMatrix(world.getCamera()->getViewMatrix());

    m_sharedState->setAmbientColor(world.getRenderingParams().getAmbientLight());
    m_sharedState->setFog(world.getRenderingParams().getFogDensity(), world.getRenderingParams().getFogColor());

    m_renderQueue->sort();

    // Ambient pass + Early Z.
    m_blendModeOverriden = true;
    m_depthTestOverriden = true;
    m_depthWriteOverriden = true;
    m_renderBackend->setBlendingMode(BlendingMode::NONE);
    m_renderBackend->enableDepthTest(true);
    m_renderBackend->enableDepthWrite(true);

    for (const auto &op : m_renderQueue->litOpaqueOperations)
    {
        m_sharedState->setWorldMatrix(op.worldTransform);

        bindPass(m_embedResources->ambientPass);

        m_renderBackend->bindVertexBuffer(op.vertexBuffer);
        if (op.indexBuffer.isValid())
            m_renderBackend->bindIndexBuffer(op.indexBuffer);

        m_renderBackend->draw(op.topology, op.numberOfElements);
    }

    // Opaque pass with lights on.
    m_renderBackend->setBlendingMode(BlendingMode::ADD);
    m_renderBackend->enableDepthWrite(false);

    for (size_t i = 0; i < LIGHTS_MAX; i++)
        m_sharedState->setLight(m_renderQueue->lights[i], i);

    for (const auto &op : m_renderQueue->litOpaqueOperations)
    {
        m_sharedState->setWorldMatrix(op.worldTransform);

        bindPass(op.passProps);

        m_renderBackend->bindVertexBuffer(op.vertexBuffer);
        if (op.indexBuffer.isValid())
            m_renderBackend->bindIndexBuffer(op.indexBuffer);

        m_renderBackend->draw(op.topology, op.numberOfElements);
    }

    // Rendering others as usual.
    m_blendModeOverriden = false;
    m_depthTestOverriden = false;
    m_depthWriteOverriden = false;

    // Opaque pass without lights.
    for (const auto &op : m_renderQueue->notLitOpaqueOperations)
        drawRenderOperation(op);

    // VFX pass.
    world.vfx().render();

    // Transparent pass.
    for (const auto &op : m_renderQueue->transparentOperations)
        drawRenderOperation(op);

    // Debug draw pass.
    if (EngineCVars::bulletDebugDraw)
    {
        for (const auto &op : m_renderQueue->debugDrawOperations)
            drawRenderOperation(op);
    }

    render2D();
}

void RenderManager::bindPass(RenderPass *pass)
{
    if (!pass)
        return;

    // Use pass program.
    m_renderBackend->bindGpuProgram(pass->program->handle);
    // Update shared uniforms.
    m_sharedState->updateSharedUniforms(*pass->program);
    // Update custom uniforms.
    pass->bindCustomPassParams(getBackend());

    // Set pass state.
    if (!m_depthTestOverriden)
        m_renderBackend->enableDepthTest(pass->depthTest);
    if (!m_depthWriteOverriden)
        m_renderBackend->enableDepthWrite(pass->depthWrite);
    if (!m_blendModeOverriden)
        m_renderBackend->setBlendingMode(pass->blendMode);
    m_renderBackend->setCullFaceMode(pass->faceCullMode);
}

void RenderManager::render2D()
{
    m_sharedState->setProjectionMatrix(glm::ortho(0.0f, (float)m_viewport.width(), (float)m_viewport.height(), 0.0f));
    m_sharedState->setViewMatrix(glm::mat4(1.0f));

    // 2D ops pass.
    for (const auto &op : m_renderQueue->sprite2DOperations)
        drawRenderOperation(op);

    // Draw GUI.
    svc().guiManager().getRenderer()->BeginPaint(m_viewport.width(), m_viewport.height());
    svc().guiManager().getRoot()->InvokePaint(tb::TBWidget::PaintProps());
    svc().guiManager().getRenderer()->EndPaint();

    // If animations are running, reinvalidate immediately
    if (tb::TBAnimationManager::HasAnimationsRunning())
        svc().guiManager().getRoot()->Invalidate();
}

RenderManager::RenderManager()
{

}

RenderManager::~RenderManager()
{

}

void RenderManager::initialize(int width, int height)
{
    m_renderQueue = make_unique<RenderQueue>();
    m_viewport = Viewport(0, 0, width, height);
    m_renderBackend = IRenderBackend::create(width, height);
    m_sharedState = make_unique<GpuProgramSharedState>();

    reloadEmbedResources();
}

void RenderManager::shutdown()
{
    m_embedResources.reset();
    m_renderBackend.reset();
}

void RenderManager::reloadEmbedResources()
{
    m_embedResources = make_unique<RenderManagerEmbedResources>(this);
}

void RenderManager::drawWorld(World &world)
{
    if (!m_renderBackend)
        return;

    onFrameBegin();

    doRenderWorld(world);

    onFrameEnd();
}

void RenderManager::drawRenderOperation(const RenderOperation &op)
{
    if (op.numberOfElements == 0)
    {
        DF3D_ASSERT_MESS(false, "invalid elements count to draw");
        return;
    }

    m_sharedState->setWorldMatrix(op.worldTransform);
    bindPass(op.passProps);

    m_renderBackend->bindVertexBuffer(op.vertexBuffer);
    if (op.indexBuffer.isValid())
        m_renderBackend->bindIndexBuffer(op.indexBuffer);

    m_renderBackend->draw(op.topology, op.numberOfElements);
}

const Viewport& RenderManager::getViewport() const
{
    return m_viewport;
}

FrameStats RenderManager::getFrameStats() const
{
    if (m_renderBackend)
        return m_renderBackend->getFrameStats();
    return {};
}

IRenderBackend& RenderManager::getBackend()
{
    return *m_renderBackend;
}

}
