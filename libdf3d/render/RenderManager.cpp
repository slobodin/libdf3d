#include "RenderManager.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/base/DebugConsole.h>
#include <libdf3d/resources/ResourceManager.h>
#include <libdf3d/resources/ResourceFactory.h>
#include <libdf3d/3d/Camera.h>
#include <libdf3d/game/World.h>
#include <libdf3d/gui/GuiManager.h>
#include "RendererBackend.h"
#include "VertexIndexBuffer.h"
#include "RenderOperation.h"
#include "MeshData.h"
#include "Texture2D.h"
#include "Material.h"
#include "Technique.h"
#include "RenderPass.h"
#include "RenderTargetScreen.h"
#include "RenderTargetTexture.h"
#include "GpuProgram.h"
#include "Viewport.h"
#include "RenderQueue.h"

namespace df3d { 

void RenderManager::createQuadRenderOperation()
{
    RenderPass passThrough;
    passThrough.setFrontFaceWinding(RenderPass::WindingOrder::CCW);
    passThrough.setFaceCullMode(RenderPass::FaceCullMode::BACK);
    passThrough.setGpuProgram(svc().resourceManager().getFactory().createRttQuadProgram());
    passThrough.setBlendMode(RenderPass::BlendingMode::NONE);
    passThrough.enableDepthTest(false);
    passThrough.enableDepthWrite(false);

    m_quadVb = createQuad(VertexFormat({ VertexFormat::POSITION_3, VertexFormat::TX_2 }), 0.0f, 0.0f, 2.0, 2.0f, GpuBufferUsageType::STATIC);
}

void RenderManager::createRenderTargets(const Viewport &vp)
{
    m_screenRt = make_unique<RenderTargetScreen>(vp);
    m_textureRt = make_unique<RenderTargetTexture>(vp);

    for (size_t i = 0; i < 2; i++)
        m_postProcessPassBuffers[i] = make_unique<RenderTargetTexture>(vp);
}

void RenderManager::createAmbientPassProps()
{
    m_ambientPassProps = make_unique<RenderPass>();

    m_ambientPassProps->setGpuProgram(svc().resourceManager().getFactory().createAmbientPassProgram());
}

void RenderManager::postProcessPass(shared_ptr<Material> material)
{
    auto tech = material->getCurrentTechnique();

    size_t passCount = tech->getPassCount();

    for (size_t passidx = 0; passidx < passCount; passidx++)
    {
        RenderTarget *rt = nullptr;

        // Last pass.
        if ((passidx + 1) == passCount)
            rt = m_screenRt.get();
        else
            rt = m_postProcessPassBuffers[passidx % 2].get();

        rt->bind();

        m_renderer->clearColorBuffer();
        m_renderer->clearDepthBuffer();

        RenderOperation quadRo;

        quadRo.vertexData = m_quadVb;
        quadRo.passProps = tech->getPass(passidx);
        quadRo.passProps->setSampler("sceneTexture", m_textureRt->getTexture());
        if (passidx != 0)
            quadRo.passProps->setSampler("prevPassBuffer", m_postProcessPassBuffers[(passidx - 1) % 2]->getTexture());

        m_renderer->drawOperation(quadRo);

        rt->unbind();
    }
}

void RenderManager::loadEmbedResources()
{
    m_renderer->loadResources();

    createRenderTargets(Viewport(0, 0, m_initParams.windowWidth, m_initParams.windowHeight));
    createQuadRenderOperation();
    createAmbientPassProps();
}

void RenderManager::onFrameBegin()
{
    m_renderer->beginFrame();
}

void RenderManager::onFrameEnd()
{
    m_renderer->endFrame();
    m_lastStats = m_stats;

    m_stats.reset();
}

void RenderManager::doRenderWorld(World &world)
{
    auto postProcessingEnabled = world.getRenderingParams().getPostProcessMaterial() != nullptr;

    RenderTarget *rt = nullptr;
    if (postProcessingEnabled)
        rt = m_textureRt.get();
    else
        rt = m_screenRt.get();

    // Draw whole scene to the texture or to the screen (depends on postprocess option).
    rt->bind();
    m_renderer->setProjectionMatrix(world.getCamera()->getProjectionMatrix());

    m_renderer->clearColorBuffer();
    m_renderer->clearDepthBuffer();

    m_stats.totalLights += m_renderQueue->lights.size();

    m_renderer->setAmbientLight(world.getRenderingParams().getAmbientLight());
    m_renderer->enableFog(world.getRenderingParams().getFogDensity(), world.getRenderingParams().getFogColor());

    m_renderer->setCameraMatrix(world.getCamera()->getViewMatrix());

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

        m_renderer->drawVertexBuffer(op.vertexData.get(), op.indexData.get(), op.type);
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
            m_renderer->bindPass(op.passProps.get());
            m_renderer->drawVertexBuffer(op.vertexData.get(), op.indexData.get(), op.type);
        }
    }

    // Rendering others as usual.
    m_renderer->disableBlendModeOverride();
    m_renderer->disableDepthTestOverride();
    m_renderer->disableDepthWriteOverride();

    // Opaque pass without lights.
    for (const auto &op : m_renderQueue->notLitOpaqueOperations)
        m_renderer->drawOperation(op);

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

    m_renderer->setProjectionMatrix(glm::ortho(0.0f, (float)rt->getViewport().width(), (float)rt->getViewport().height(), 0.0f));
    m_renderer->setCameraMatrix(glm::mat4(1.0f));

    // 2D ops pass.
    for (const auto &op : m_renderQueue->sprite2DOperations)
        m_renderer->drawOperation(op);

    // Do post process if enabled.
    if (postProcessingEnabled)
    {
        rt->unbind();
        postProcessPass(world.getRenderingParams().getPostProcessMaterial());
    }

    // Draw GUI.
    m_screenRt->bind();

    m_renderer->setProjectionMatrix(glm::ortho(0.0f, (float)m_screenRt->getViewport().width(), (float)m_screenRt->getViewport().height(), 0.0f));
    m_renderer->setCameraMatrix(glm::mat4(1.0f));

    svc().guiManager().getContext()->Render();
}

RenderManager::RenderManager(EngineInitParams params)
    : m_renderQueue(make_unique<RenderQueue>()),
    m_initParams(params)
{
    m_renderer = make_unique<RendererBackend>();
    m_renderer->setRenderStatsLocation(&m_stats);
}

RenderManager::~RenderManager()
{

}

void RenderManager::drawWorld(World &world)
{
    // TODO_ecs: what if render queue becomes big???
    m_renderQueue->clear();

    onFrameBegin();

    world.collectRenderOperations(m_renderQueue.get());
    doRenderWorld(world);

    onFrameEnd();
}

const RenderStats& RenderManager::getLastRenderStats() const
{
    return m_lastStats;
}

const RenderTargetScreen& RenderManager::getScreenRenderTarget() const
{
    return *m_screenRt;
}

const RenderingCapabilities& RenderManager::getRenderingCapabilities() const
{
    return m_initParams.renderingCaps;
}

RendererBackend* RenderManager::getRenderer()
{
    return m_renderer.get();
}

}
