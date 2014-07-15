#include "df3d_pch.h"
#include "RenderManager.h"

#include <base/Controller.h>
#include <scene/Scene.h>
#include <scene/Camera.h>
#include <gui/GuiManager.h>
#include <resources/ResourceManager.h>
#include <physics/PhysicsManager.h>
#include "Renderer.h"
#include "VertexIndexBuffer.h"
#include "RenderOperation.h"
#include "MeshData.h"
#include "Texture.h"
#include "Material.h"
#include "Technique.h"
#include "RenderPass.h"
#include "RenderTargetScreen.h"
#include "RenderTargetTexture.h"
#include "GpuProgram.h"
#include "Viewport.h"
#include "SubMesh.h"
#include "Image.h"
#include "RenderQueue.h"

namespace df3d { namespace render {

void RenderManager::createQuadRenderOperation()
{
    auto passThrough = make_shared<RenderPass>();
    passThrough->setFrontFaceWinding(RenderPass::WO_CCW);
    passThrough->setFaceCullMode(RenderPass::FCM_BACK);
    auto program = g_resourceManager->getResource<GpuProgram>(RTT_QUAD_PROGRAM_EMBED_PATH);
    passThrough->setGpuProgram(program);
    passThrough->setBlendMode(RenderPass::BM_NONE);
    passThrough->enableDepthTest(false);
    passThrough->enableDepthWrite(false);

    // Create vertex buffer.
    float quad_pos[][2] = {
        { -1.0, -1.0 },
        { 1.0, -1.0 },
        { 1.0, 1.0 },
        { 1.0, 1.0 },
        { -1.0, 1.0 },
        { -1.0, -1.0 }
    };
    float quad_uv[][2] = {
        { 0.0, 0.0 },
        { 1.0, 0.0 },
        { 1.0, 1.0 },
        { 1.0, 1.0 },
        { 0.0, 1.0 },
        { 0.0, 0.0 }
    };

    m_quadVb = make_shared<VertexBuffer>(VertexFormat::create("p:3, tx:2"));
    m_quadVb->setUsageType(GB_USAGE_STATIC);

    for (int i = 0; i < 6; i++)
    {
        render::Vertex_3p2tx v;
        v.p.x = quad_pos[i][0];
        v.p.y = quad_pos[i][1];
        v.tx.x = quad_uv[i][0];
        v.tx.y = quad_uv[i][1];

        m_quadVb->appendVertexData((const float *)&v, 1);
    }

    m_defaultPostProcessMaterial = shared_ptr<render::Material>(new render::Material("default_postprocess_material"));
    auto defaultTech = shared_ptr<render::Technique>(new render::Technique("default_technique"));

    defaultTech->appendPass(passThrough);
    m_defaultPostProcessMaterial->appendTechnique(defaultTech);
    m_defaultPostProcessMaterial->setCurrentTechnique("default_technique");
}

void RenderManager::createRenderTargets()
{
    // Create screen RT.
    m_screenRt = make_shared<RenderTargetScreen>();

    m_textureRt = make_shared<RenderTargetTexture>(createOffscreenBuffer());

    for (size_t i = 0; i < MAX_POSPROCESS_PASSES; i++)
        // Doesn't actually create gpu buffer. Created when it's being used.
        m_postProcessPassBuffers[i] = make_shared<RenderTargetTexture>(createOffscreenBuffer());
}

shared_ptr<Texture> RenderManager::createOffscreenBuffer()
{
    // Create texture RT. Used for post process scene effects.
    auto offscreenBuffer = make_shared<Texture>();
    offscreenBuffer->setMipmapped(false);
    offscreenBuffer->setFilteringMode(Texture::NEAREST);
    offscreenBuffer->setType(Texture::TEXTURE_2D);

    auto image = make_shared<Image>();

    // FIXME:
    // Recreate when size of viewport is changed!
    image->setWidth(m_viewport->width());
    image->setHeight(m_viewport->height());
    image->setPixelFormat(Image::PF_RGBA);
    image->setInitialized();

    offscreenBuffer->setImage(image);

    return offscreenBuffer;
}

void RenderManager::createAmbientPassProps()
{
    m_ambientPassProps = make_shared<RenderPass>();

    auto program = g_resourceManager->getResource<render::GpuProgram>(AMBIENT_PASS_PROGRAM_EMBED_PATH);
    m_ambientPassProps->setGpuProgram(program);
}

void RenderManager::debugDrawPass()
{
    // Draw scene graph debug draw nodes.
    for (auto &op : m_renderQueue->debugDrawOperations)
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

        setupViewport(rt);

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

//void RenderManager::collectNodes(shared_ptr<scene::Node> node)
//{
//    if (auto pMesh = boost::dynamic_pointer_cast<scene::MeshNode>(node))
//    {
//
//
//        ++m_stats.totalNodes;
//
//        return;
//    }
//
//    if (auto pParticleSystem = boost::dynamic_pointer_cast<particlesys::ParticleSystemNode>(node))
//    {
//
//        ++m_stats.totalParticleSystems;
//        m_stats.totalParticles += pParticleSystem->getParticlesCount();
//
//        return;
//    }
//}

RenderManager::RenderManager()
    : m_renderQueue(new RenderQueue())
{
}

RenderManager::~RenderManager()
{
}

bool RenderManager::init(RenderManagerInitParams params)
{
    m_viewport = make_shared<Viewport>();
    m_viewport->setDimensions(0, 0, params.viewportWidth, params.viewportHeight);

#if defined(__WIN32__) || defined(__ANDROID__)
    m_renderer.reset(new Renderer());
#else
#error "Unsupported platform"
#endif
    if (!m_renderer->initialize())
        return false;

    m_renderer->setRenderStatsLocation(&m_stats);

    createRenderTargets();
    createQuadRenderOperation();
    createAmbientPassProps();

    enableDebugDraw(params.debugDraw);

    return true;
} 

void RenderManager::shutdown()
{
    m_renderQueue->clear();
    m_renderer->shutdown();
}

void RenderManager::enableDebugDraw(bool enable)
{
    m_debugDrawEnabled = enable;
}

bool RenderManager::isDebugDrawEnabled()
{
    return m_debugDrawEnabled;
}

void RenderManager::update(shared_ptr<scene::Scene> renderableScene)
{
    if (!renderableScene)
        return;

    renderableScene->collectRenderOperations(m_renderQueue.get());
}

void RenderManager::drawScene(shared_ptr<scene::Scene> sc, shared_ptr<scene::Camera> camera)
{
    if (!sc || !camera)
        return;

    auto postProcessingEnabled = sc->getPostProcessMaterial() != nullptr;

    shared_ptr<RenderTarget> rt;
    if (postProcessingEnabled)
        rt = m_textureRt;
    else
        rt = m_screenRt;

    // Draw whole scene to the texture or to the screen (depends on postprocess option).
    setupViewport(rt);
    m_renderer->setProjectionMatrix(camera->getProjectionMatrix());

    m_renderer->clearColorBuffer();
    m_renderer->clearDepthBuffer();

    m_stats.totalLights += m_renderQueue->lights.size();

    m_renderer->setAmbientLight(sc->getAmbientLight());
    m_renderer->enableFog(sc->getFogDensity(), sc->getFogColor());

    m_renderer->setCameraMatrix(camera->getMatrix());

    m_renderQueue->sort();

    // Ambient pass.
    m_renderer->enableBlendModeOverride(RenderPass::BM_NONE);
    m_renderer->enableDepthTestOverride(true);
    m_renderer->enableDepthWriteOverride(true);

    for (const auto &op : m_renderQueue->litOpaqueOperations)
    {
        m_renderer->setWorldMatrix(op.worldTransform);

        m_ambientPassProps->setAmbientColor(op.passProps->getAmbientColor());
        m_ambientPassProps->setEmissiveColor(op.passProps->getEmissiveColor());

        m_renderer->bindPass(m_ambientPassProps);

        m_renderer->drawVertexBuffer(op.vertexData, op.indexData, op.type);
    }

    // Opaque pass with lights on.
    m_renderer->enableBlendModeOverride(RenderPass::BM_ADD);
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
            m_renderer->drawVertexBuffer(op.vertexData, op.indexData, op.type);
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
    for (const auto &op : m_renderQueue->transparentOperations)
        drawOperation(op);

    // Debug draw pass.
    if (m_debugDrawEnabled)
        debugDrawPass();

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
    m_renderer->setWorldMatrix(op.worldTransform);
    m_renderer->bindPass(op.passProps);
    m_renderer->drawVertexBuffer(op.vertexData, op.indexData, op.type);
}

void RenderManager::drawGUI()
{
    setupViewport(m_screenRt);

    m_renderer->setProjectionMatrix(glm::ortho(0.0f, (float)m_screenRt->getWidth(), (float)m_screenRt->getHeight(), 0.0f));
    m_renderer->setCameraMatrix(glm::mat4(1.0f));

    g_guiManager->render();
}

void RenderManager::setupViewport(shared_ptr<RenderTarget> rt)
{
    rt->bind();

    auto w = rt->getWidth();
    auto h = rt->getHeight();

    m_viewport->setDimensions(0, 0, w, h);

    m_renderer->setViewport((unsigned int)w, (unsigned int)h);
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

shared_ptr<Viewport> RenderManager::getViewport()
{
    return m_viewport;
}

Renderer *RenderManager::getRenderer() const
{
    return m_renderer.get();
}

} }