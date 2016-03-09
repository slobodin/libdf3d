#include "RenderManager.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/base/FrameStats.h>
#include <libdf3d/base/DebugConsole.h>
#include <libdf3d/resources/ResourceManager.h>
#include <libdf3d/resources/ResourceFactory.h>
#include <libdf3d/3d/Camera.h>
#include <libdf3d/game/World.h>
#include <libdf3d/gui/GuiManager.h>
#include <libdf3d/particlesys/ParticleSystemComponentProcessor.h>
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

namespace df3d {

void RenderManager::createQuadRenderOperation()
{
    RenderPass passThrough;
    passThrough.setFaceCullMode(RenderPass::FaceCullMode::BACK);
    passThrough.setGpuProgram(svc().resourceManager().getFactory().createRttQuadProgram());
    passThrough.setBlendMode(RenderPass::BlendingMode::NONE);
    passThrough.enableDepthTest(false);
    passThrough.enableDepthWrite(false);

    // TODO_render:
    /*
    m_quadVb = createQuad(VertexFormat({ VertexFormat::POSITION_3, VertexFormat::TX_2 }), 0.0f, 0.0f, 2.0, 2.0f, GpuBufferUsageType::STATIC);
    */
}

void RenderManager::createAmbientPassProps()
{
    m_ambientPassProps = make_unique<RenderPass>();

    m_ambientPassProps->setGpuProgram(svc().resourceManager().getFactory().createAmbientPassProgram());
}

void RenderManager::loadEmbedResources()
{
    m_renderBackend->createEmbedResources();

    createQuadRenderOperation();
    createAmbientPassProps();
}

void RenderManager::onFrameBegin()
{
    m_renderBackend->clearColorBuffer();
    m_renderBackend->clearDepthBuffer();
    m_renderBackend->clearStencilBuffer();
    m_renderBackend->enableDepthTest(true);
    m_renderBackend->enableDepthWrite(true);
    m_renderBackend->enableScissorTest(false);

    // TODO_render:
    /*
    m_renderBackend->setBlendMode(RenderPass::BlendingMode::NONE);
    */
}

void RenderManager::onFrameEnd()
{
    // TODO_render:
    //m_renderer->endFrame();
}

void RenderManager::doRenderWorld(World &world)
{
    // TODO_render
    /*

    // TODO_ecs: what if render queue becomes big???
    m_renderQueue->clear();

    world.collectRenderOperations(m_renderQueue.get());

    m_renderBackend->setProjectionMatrix(world.getCamera()->getProjectionMatrix());

    m_renderBackend->clearColorBuffer();
    m_renderBackend->clearDepthBuffer();

    m_renderer->setAmbientLight(world.getRenderingParams().getAmbientLight());
    m_renderer->enableFog(world.getRenderingParams().getFogDensity(), world.getRenderingParams().getFogColor());

    m_renderBackend->setCameraMatrix(world.getCamera()->getViewMatrix());

    m_renderQueue->sort();

    // Ambient pass.
    m_renderer->enableBlendModeOverride(RenderPass::BlendingMode::NONE);
    m_renderer->enableDepthTestOverride(true);
    m_renderer->enableDepthWriteOverride(true);

    for (const auto &op : m_renderQueue->litOpaqueOperations)
    {
        m_renderer->setWorldMatrix(op.worldTransform);

        m_ambientPassProps->setAmbientColor(op.passProps->getAmbientColor());
        m_ambientPassProps->setEmissiveColor(op.passProps->getEmissiveColor());

        m_renderer->bindPass(m_ambientPassProps.get());

        m_renderer->drawVertexBuffer(op.vertexData, op.indexData, op.type);
    }

    // Opaque pass with lights on.
    m_renderer->enableBlendModeOverride(RenderPass::BlendingMode::ADD);
    m_renderer->enableDepthWriteOverride(false);

    for (const auto &op : m_renderQueue->litOpaqueOperations)
    {
        m_renderer->setWorldMatrix(op.worldTransform);
        // TODO: bind pass only once

        for (const auto &light : m_renderQueue->lights)
        {
            m_renderer->setLight(*light);

            // TODO: update ONLY light uniforms.
            m_renderer->bindPass(op.passProps);
            m_renderer->drawVertexBuffer(op.vertexData, op.indexData, op.type);
        }
    }

    // Rendering others as usual.
    m_renderer->disableBlendModeOverride();
    m_renderer->disableDepthTestOverride();
    m_renderer->disableDepthWriteOverride();

    // Opaque pass without lights.
    for (const auto &op : m_renderQueue->notLitOpaqueOperations)
        m_renderer->drawOperation(op);

    // VFX pass.
    world.vfx().draw();

    // Transparent pass.
    for (const auto &op : m_renderQueue->transparentOperations)
        m_renderer->drawOperation(op);

    // Debug draw pass.
    if (auto console = svc().debugConsole())
    {
        if (console->getCVars().get<bool>(CVAR_DEBUG_DRAW))
        {
            for (const auto &op : m_renderQueue->debugDrawOperations)
                m_renderer->drawOperation(op);
        }
    }

    m_renderBackend->setProjectionMatrix(glm::ortho(0.0f, (float)m_viewport.width(), (float)m_viewport.height(), 0.0f));
    m_renderBackend->setCameraMatrix(glm::mat4(1.0f));

    // 2D ops pass.
    for (const auto &op : m_renderQueue->sprite2DOperations)
        m_renderer->drawOperation(op);

    // Draw GUI.
    m_renderBackend->setProjectionMatrix(glm::ortho(0.0f, (float)m_viewport.width(), (float)m_viewport.height(), 0.0f));
    m_renderBackend->setCameraMatrix(glm::mat4(1.0f));

    svc().guiManager().getContext()->Render();

    */
}

RenderManager::RenderManager(EngineInitParams params)
    : m_renderQueue(make_unique<RenderQueue>()),
    m_initParams(params)
{
    m_viewport = Viewport(0, 0, params.windowWidth, params.windowHeight);

    m_renderBackend = IRenderBackend::create();
}

RenderManager::~RenderManager()
{

}

void RenderManager::drawWorld(World &world)
{
    onFrameBegin();

    doRenderWorld(world);

    onFrameEnd();
}

const Viewport& RenderManager::getViewport() const
{
    return m_viewport;
}

const RenderingCapabilities& RenderManager::getRenderingCapabilities() const
{
    return m_initParams.renderingCaps;
}

IRenderBackend& RenderManager::getBackend()
{
    return *m_renderBackend;
}

}
