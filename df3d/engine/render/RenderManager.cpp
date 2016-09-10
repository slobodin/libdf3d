#include "RenderManager.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/EngineCVars.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceFactory.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/game/World.h>
#include <df3d/engine/gui/GuiManager.h>
#include <df3d/engine/particlesys/ParticleSystemComponentProcessor.h>
#include "RenderOperation.h"
#include "MeshData.h"
#include "Texture.h"
#include "Material.h"
#include "Technique.h"
#include "RenderPass.h"
#include "GpuProgram.h"
#include "Viewport.h"
#include "RenderQueue.h"
#include "IRenderBackend.h"
#include "GpuProgramSharedState.h"

#include <tb_widgets.h>
#include <animation/tb_widget_animation.h>

namespace df3d {

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

        m_ambientPassProps->getPassParam(m_ambientMtlParam)->setValue(op.passProps->getAmbientColor());

        bindPass(m_ambientPassProps.get());

        m_renderBackend->bindVertexBuffer(op.vertexBuffer);
        if (op.indexBuffer.isValid())
            m_renderBackend->bindIndexBuffer(op.indexBuffer);

        m_renderBackend->draw(op.topology, op.numberOfElements);
    }

    // Opaque pass with lights on.
    m_renderBackend->setBlendingMode(BlendingMode::ADD);
    m_renderBackend->enableDepthWrite(false);

    for (const auto &op : m_renderQueue->litOpaqueOperations)
    {
        if (UNLIKELY(!op.passProps->getGpuProgram()))
            continue;

        m_sharedState->setWorldMatrix(op.worldTransform);

        bindPass(op.passProps);

        for (const auto &light : m_renderQueue->lights)
        {
            m_sharedState->setLight(*light);
            // Update only light uniforms.
            m_sharedState->updateSharedLightUniforms(*op.passProps->getGpuProgram());

            m_renderBackend->bindVertexBuffer(op.vertexBuffer);
            if (op.indexBuffer.isValid())
                m_renderBackend->bindIndexBuffer(op.indexBuffer);
            m_renderBackend->draw(op.topology, op.numberOfElements);
        }
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
    if (UNLIKELY(!pass))
        return;

    auto gpuProgram = pass->getGpuProgram();
    if (UNLIKELY(!gpuProgram))
    {
        DFLOG_WARN("Failed to bind pass. No GPU program");
        return;
    }

    m_renderBackend->bindGpuProgram(gpuProgram->getHandle());

    // Update pass uniforms.
    {
        // Shared uniforms.
        m_sharedState->updateSharedUniforms(*gpuProgram);

        int textureUnit = 0;
        // Custom uniforms.
        auto &passParams = pass->getPassParams();
        for (auto &passParam : passParams)
        {
            if (passParam.hasTexture())
            {
                auto texture = passParam.getTexture();
                if (texture && texture->isInitialized())
                    m_renderBackend->bindTexture(texture->getHandle(), textureUnit);
                else
                    m_renderBackend->bindTexture(m_whiteTexture->getHandle(), textureUnit);

                passParam.setValue(textureUnit);

                textureUnit++;
            }

            passParam.updateToProgram(*m_renderBackend, *gpuProgram);
        }
    }

    if (!m_depthTestOverriden)
        m_renderBackend->enableDepthTest(pass->isDepthTestEnabled());
    if (!m_depthWriteOverriden)
        m_renderBackend->enableDepthWrite(pass->isDepthWriteEnabled());
    if (!m_blendModeOverriden)
        m_renderBackend->setBlendingMode(pass->getBlendingMode());
    m_renderBackend->setCullFaceMode(pass->getFaceCullMode());
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

RenderManager::RenderManager(EngineInitParams params)
    : m_initParams(params),
    m_renderQueue(make_unique<RenderQueue>())
{
    m_viewport = Viewport(0, 0, params.windowWidth, params.windowHeight);
}

RenderManager::~RenderManager()
{

}

void RenderManager::initialize()
{
    m_renderBackend = IRenderBackend::create(m_initParams.windowWidth, m_initParams.windowHeight);
    m_sharedState = make_unique<GpuProgramSharedState>();

    loadEmbedResources();
}

void RenderManager::shutdown()
{
    forgetEmbedResources();
}

void RenderManager::loadEmbedResources()
{
    // Create white texture.
    {
        const auto w = 8;
        const auto h = 8;
        const auto pf = PixelFormat::RGBA;

        auto data = new uint8_t[w * h * 4];
        memset(data, 255, w * h * 4);

        TextureInfo info;
        info.width = w;
        info.height = h;
        info.flags = TEXTURE_FILTERING_NEAREST | TEXTURE_WRAP_MODE_REPEAT;
        info.numMips = 0;
        info.format = pf;

        m_whiteTexture = svc().resourceManager().getFactory().createTexture(info, data, w * h * 4);
        m_whiteTexture->setResident(true);

        delete [] data;
    }

    // Load resident GPU programs.
    {
        const std::string simple_lighting_vert =
#include "gl/embed_glsl/simple_lighting_vert.h"
            ;
        const std::string simple_lighting_frag =
#include "gl/embed_glsl/simple_lighting_frag.h"
            ;
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

        auto &factory = svc().resourceManager().getFactory();

        m_simpleLightingProgram = factory.createGpuProgram(SIMPLE_LIGHTING_PROGRAM_EMBED_PATH, simple_lighting_vert, simple_lighting_frag);
        m_coloredProgram = factory.createGpuProgram(COLORED_PROGRAM_EMBED_PATH, colored_vert, colored_frag);
        m_ambientPassProgram = factory.createGpuProgram(AMBIENT_PASS_PROGRAM_EMBED_PATH, ambient_vert, ambient_frag);

        m_simpleLightingProgram->setResident(true);
        m_coloredProgram->setResident(true);
        m_ambientPassProgram->setResident(true);
    }

    m_ambientPassProps = make_unique<RenderPass>();
    m_ambientMtlParam = m_ambientPassProps->getPassParamHandle("material_ambient");
    m_ambientPassProps->setGpuProgram(svc().resourceManager().getFactory().createAmbientPassProgram());
}

void RenderManager::forgetEmbedResources()
{
    if (m_whiteTexture)
        m_whiteTexture->setResident(false);
    if (m_simpleLightingProgram)
        m_simpleLightingProgram->setResident(false);
    if (m_coloredProgram)
        m_coloredProgram->setResident(false);
    if (m_ambientPassProgram)
        m_ambientPassProgram->setResident(false);

    m_whiteTexture.reset();
    m_ambientPassProps.reset();
    m_simpleLightingProgram.reset();
    m_coloredProgram.reset();
    m_ambientPassProgram.reset();
}

void RenderManager::drawWorld(World &world)
{
    if (UNLIKELY(!m_renderBackend))
        return;

    onFrameBegin();

    doRenderWorld(world);

    onFrameEnd();
}

void RenderManager::drawRenderOperation(const RenderOperation &op)
{
    if (UNLIKELY(op.numberOfElements == 0))
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
