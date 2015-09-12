#include "df3d_pch.h"
#include "RenderManager.h"

#include <resources/ResourceFactory.h>
#include <base/SystemsMacro.h>
#include <base/DebugConsole.h>
#include <scene/Scene.h>
#include <scene/Camera.h>
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

namespace df3d { namespace render {

void RenderManager::createQuadRenderOperation()
{
    RenderPass passThrough;
    passThrough.setFrontFaceWinding(RenderPass::WindingOrder::CCW);
    passThrough.setFaceCullMode(RenderPass::FaceCullMode::BACK);
    passThrough.setGpuProgram(g_resourceManager->getFactory().createRttQuadProgram());
    passThrough.setBlendMode(RenderPass::BlendingMode::NONE);
    passThrough.enableDepthTest(false);
    passThrough.enableDepthWrite(false);

    m_quadVb = render::createQuad(VertexFormat::create("p:3, tx:2"), 0.0f, 0.0f, 2.0, 2.0f, GpuBufferUsageType::STATIC);

    m_defaultPostProcessMaterial = shared_ptr<render::Material>(new render::Material("default_postprocess_material"));

    Technique defaultTech("default_technique");
    defaultTech.appendPass(passThrough);

    m_defaultPostProcessMaterial->appendTechnique(defaultTech);
    m_defaultPostProcessMaterial->setCurrentTechnique("default_technique");
}

void RenderManager::createRenderTargets(const Viewport &vp)
{
    // Create screen RT.
    m_screenRt = make_shared<RenderTargetScreen>(vp);

    m_textureRt = make_shared<RenderTargetTexture>(vp);

    for (size_t i = 0; i < MAX_POSPROCESS_PASSES; i++)
        // Doesn't actually create gpu buffer. Created when it's being used.
        m_postProcessPassBuffers[i] = make_shared<RenderTargetTexture>(vp);
}

void RenderManager::createAmbientPassProps()
{
    m_ambientPassProps = make_unique<RenderPass>();

    m_ambientPassProps->setGpuProgram(g_resourceManager->getFactory().createAmbientPassProgram());
}

void RenderManager::debugDrawPass()
{
    if (auto console = g_engineController->getConsole())
    {
        if (!console->getCVars().get<bool>(base::CVAR_DEBUG_DRAW))
            return;
    }

    // Draw scene graph debug draw nodes.
    for (const auto &op : m_renderQueue->debugDrawOperations)
        drawOperation(op);

    // Draw bullet physics debug.
    g_engineController->getPhysicsManager()->drawDebug();
}

void RenderManager::postProcessPass(shared_ptr<Material> material)
{
    //m_renderer->clearDepthBuffer();

    auto tech = material->getCurrentTechnique();
    if (!tech)
    {
        base::glog << "Post process technique was not set. Use default." << base::logdebug;
        tech = m_defaultPostProcessMaterial->getCurrentTechnique();
    }

    size_t passCount = tech->getPassCount();
    if (passCount > MAX_POSPROCESS_PASSES)
    {
        base::glog << "Too many post process passes" << base::logdebug;
        return;
    }

    for (size_t passidx = 0; passidx < passCount; passidx++)
    {
        shared_ptr<RenderTarget> rt;

        // Last pass.
        if ((passidx + 1) == passCount)
            rt = m_screenRt;
        else
            rt = m_postProcessPassBuffers[passidx];

        rt->bind();

        m_renderer->clearColorBuffer();

        RenderOperation quadRo;

        quadRo.vertexData = m_quadVb;
        quadRo.passProps = tech->getPass(passidx);
        quadRo.passProps->setSampler("sceneTexture", m_textureRt->getTexture());
        if (passidx != 0)
            quadRo.passProps->setSampler("prevPassBuffer", m_postProcessPassBuffers[passidx - 1]->getTexture());

        drawOperation(quadRo);

        rt->unbind();
    }
}

void RenderManager::loadEmbedResources()
{
    m_renderer->loadResources();

    createRenderTargets(Viewport(0, 0, m_initParams.viewportWidth, m_initParams.viewportHeight));
    createQuadRenderOperation();
    createAmbientPassProps();
}

RenderManager::RenderManager(RenderManagerInitParams params)
    : m_renderQueue(make_unique<RenderQueue>()),
    m_initParams(params)
{
    m_renderer = make_unique<RendererBackend>();
    m_renderer->setRenderStatsLocation(&m_stats);
}

RenderManager::~RenderManager()
{

}

void RenderManager::update(shared_ptr<scene::Scene> renderableScene)
{
    if (!renderableScene)
        return;

    renderableScene->collectStats(&m_stats);
    renderableScene->collectRenderOperations(m_renderQueue.get());
}

void RenderManager::drawScene(shared_ptr<scene::Scene> sc)
{
    if (!sc)
        return;

    auto camera = sc->getCamera();
    if (!camera)
    {
        base::glog << "Can not draw scene. Camera is invalid." << base::logwarn;
        return;
    }

    auto postProcessingEnabled = sc->getPostProcessMaterial() != nullptr;

    shared_ptr<RenderTarget> rt;
    if (postProcessingEnabled)
        rt = m_textureRt;
    else
        rt = m_screenRt;

    // Draw whole scene to the texture or to the screen (depends on postprocess option).
    rt->bind();
    m_renderer->setProjectionMatrix(camera->getProjectionMatrix());

    m_renderer->clearColorBuffer();
    m_renderer->clearDepthBuffer();

    m_stats.totalLights += m_renderQueue->lights.size();

    m_renderer->setAmbientLight(sc->getAmbientLight());
    m_renderer->enableFog(sc->getFogDensity(), sc->getFogColor());

    m_renderer->setCameraMatrix(camera->getViewMatrix());

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

        const auto &lights = m_renderQueue->lights;
        size_t lightsSize = lights.size();
        for (size_t lightIdx = 0; lightIdx < lightsSize; lightIdx++)
        {
            m_renderer->setLight(lights[lightIdx]);

            // TODO: update ONLY light uniforms.
            m_renderer->bindPass(op.passProps);
            m_renderer->drawVertexBuffer(op.vertexData.get(), op.indexData.get(), op.type);
        }
    }

    // Rendering others as usual.
    m_renderer->disableBlendModeOverride();
    m_renderer->disableDepthTestOverride();
    m_renderer->disableDepthWriteOverride();

    // Opaque pass without lights.
    for (const auto &op : m_renderQueue->notLitOpaqueOperations)
        drawOperation(op);

    // Transparent pass.
    // TODO: sort by Z.
    for (const auto &op : m_renderQueue->transparentOperations)
        drawOperation(op);

    // Debug draw pass.
    debugDrawPass();

    m_renderer->setProjectionMatrix(glm::ortho(0.0f, (float)rt->getViewport().width(), (float)rt->getViewport().height(), 0.0f));
    m_renderer->setCameraMatrix(glm::mat4(1.0f));

    // 2D ops pass.
    for (const auto &op : m_renderQueue->sprite2DOperations)
        drawOperation(op);

    // Do post process if enabled.
    if (postProcessingEnabled)
    {
        rt->unbind();
        postProcessPass(sc->getPostProcessMaterial());
    }

    m_renderQueue->clear();
}

void RenderManager::drawOperation(const RenderOperation &op)
{
    if (op.isEmpty())
        return;

    m_renderer->setWorldMatrix(op.worldTransform);
    m_renderer->bindPass(op.passProps);
    m_renderer->drawVertexBuffer(op.vertexData.get(), op.indexData.get(), op.type);
}

void RenderManager::drawGUI()
{
    m_screenRt->bind();

    m_renderer->setProjectionMatrix(glm::ortho(0.0f, (float)m_screenRt->getViewport().width(), (float)m_screenRt->getViewport().height(), 0.0f));
    m_renderer->setCameraMatrix(glm::mat4(1.0f));

    g_guiManager->render();
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

const RenderStats &RenderManager::getLastRenderStats() const
{
    return m_lastStats;
}

shared_ptr<RenderTargetScreen> RenderManager::getScreenRenderTarget() const
{
    return m_screenRt;
}

const RenderingCapabilities &RenderManager::getRenderingCapabilities() const
{
    return m_initParams.renderingCaps;
}

RendererBackend *RenderManager::getRenderer() const
{
    return m_renderer.get();
}

} }
