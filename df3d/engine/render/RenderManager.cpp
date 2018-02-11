#include <df3d_pch.h>
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
#include "RenderQueue.h"
#include "IRenderBackend.h"
#include "IGpuProgramSharedState.h"
#include <glm/gtc/matrix_transform.hpp>
#include <tb_widgets.h>
#include <animation/tb_widget_animation.h>

#ifdef DF3D_IOS

#include <df3d/engine/render/metal/RenderBackendMetal.h>
#define DF3D_USE_METAL_BACKEND 1

#else

#include <df3d/engine/render/gl/RenderBackendGL.h>

#endif

namespace df3d {

namespace {

bool g_usingAmbientPass = true;

static unique_ptr<IRenderBackend> CreateRenderBackend(const EngineInitParams &params)
{
#if DF3D_USE_METAL_BACKEND
    g_usingAmbientPass = false;
    return make_unique<RenderBackendMetal>(params);
#else
    g_usingAmbientPass = false;
    return make_unique<RenderBackendGL>(params.windowWidth, params.windowHeight);
#endif
}

}

RenderManagerEmbedResources::RenderManagerEmbedResources(RenderManager *render)
{
    auto &allocator = MemoryManager::allocDefault();

    // Create white texture.
    {
        const auto w = 8;
        const auto h = 8;

        TextureResourceData resource;
        resource.format = PixelFormat::RGBA;
        TextureResourceData::MipLevel mipLevel;
        mipLevel.width = w;
        mipLevel.height = h;
        mipLevel.pixels.resize(w * h * 4, 255);
        resource.mipLevels.push_back(std::move(mipLevel));

        whiteTexture = render->getBackend().createTexture(resource, TEXTURE_FILTERING_NEAREST | TEXTURE_WRAP_MODE_REPEAT);
    }

    // Load GPU programs.
    {
        if (render->getBackendID() == RenderBackendID::GL)
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

            coloredProgram = CreateGPUProgramFromData(colored_vert, colored_frag,
            {
                "material_diffuse",
                "u_worldViewProjectionMatrix",
                "diffuseMap"
            }, allocator, {}, {});

            ambientPassProgram = CreateGPUProgramFromData(ambient_vert, ambient_frag,
            {
                "u_worldViewProjectionMatrix",
                "u_globalAmbient"
            }, allocator, {}, {});
        }
        else
        {
            coloredProgram = CreateGPUProgramMetal("ColoredPass_VertexMain", "ColoredPass_FragmentMain",
                                                   {
                                                       "material_diffuse",
                                                       "u_worldViewProjectionMatrix",
                                                       "diffuseMap"
                                                   }, allocator);

            ambientPassProgram = CreateGPUProgramMetal("AmbientPass_VertexMain", "AmbientPass_FragmentMain",
                                                       {
                                                           "u_worldViewProjectionMatrix",
                                                           "u_globalAmbient"
                                                       }, allocator);
        }
    }

    ambientPass = MAKE_NEW(allocator, RenderPass)();
    ambientPass->program = ambientPassProgram;
}

RenderManagerEmbedResources::~RenderManagerEmbedResources()
{
    auto &allocator = MemoryManager::allocDefault();
    svc().renderManager().getBackend().destroyTexture(whiteTexture);
    svc().renderManager().getBackend().destroyGPUProgram(coloredProgram->handle);
    svc().renderManager().getBackend().destroyGPUProgram(ambientPassProgram->handle);
    MAKE_DELETE(allocator, coloredProgram);
    MAKE_DELETE(allocator, ambientPassProgram);
    MAKE_DELETE(allocator, ambientPass);
}

void RenderManager::onFrameBegin()
{
    m_passStateOverriden = false;
    m_overridenState = 0;

    m_sharedState->initialize(m_renderBackend.get());
    m_renderBackend->frameBegin();
    m_renderBackend->setScissorTest(false, {});
}

void RenderManager::onFrameEnd()
{
    m_renderBackend->frameEnd();
}

void RenderManager::doRenderWorld(World &world)
{
    m_renderQueue->clear();

    world.collectRenderOperations(m_renderQueue.get());

    m_renderBackend->setViewport(m_viewport);

    m_sharedState->setViewPort(m_viewport);
    m_sharedState->setProjectionMatrix(world.getCamera()->getProjectionMatrix());
    m_sharedState->setViewMatrix(world.getCamera()->getViewMatrix());
    m_sharedState->setAmbientColor(world.getRenderingParams().getAmbientLight());
    m_sharedState->setFog(world.getRenderingParams().getFogDensity(), world.getRenderingParams().getFogColor());

    m_renderQueue->sort();

    for (size_t i = 0; i < LIGHTS_MAX; i++)
        m_sharedState->setLight(m_renderQueue->lights[i], i);

    // Ambient pass + Early Z.
    if (g_usingAmbientPass)
    {
        m_passStateOverriden = true;

        m_overridenState = RENDER_STATE_DEPTH_LEQUAL | RENDER_STATE_DEPTH_WRITE;

        for (const auto &op : m_renderQueue->rops[RQ_BUCKET_LIT])
            drawRenderOperation(op, m_embedResources->ambientPass);

        // Opaque pass with lights on.
        m_overridenState = RENDER_STATE_DEPTH_LEQUAL | BLENDING_ADD;
    }

    for (const auto &op : m_renderQueue->rops[RQ_BUCKET_LIT])
        drawRenderOperation(op);

    // Rendering others as usual.
    m_passStateOverriden = false;

    // Opaque pass without lights.
    for (const auto &op : m_renderQueue->rops[RQ_BUCKET_NOT_LIT])
        drawRenderOperation(op);

    // VFX pass.
    world.vfx().render();

    // Transparent pass.
    for (const auto &op : m_renderQueue->rops[RQ_BUCKET_TRANSPARENT])
        drawRenderOperation(op);

    // Debug draw pass.
    if (EngineCVars::bulletDebugDraw)
    {
        for (const auto &op : m_renderQueue->rops[RQ_BUCKET_DEBUG])
            drawRenderOperation(op);
    }

    render2D();
}

void RenderManager::bindPass(RenderPass *pass)
{
    if (!pass)
        return;

    // Use pass program.
    m_renderBackend->bindGPUProgram(pass->program->handle);
    // Update shared uniforms.
    m_sharedState->updateSharedUniforms(*pass->program);
    // Update custom uniforms.
    pass->bindCustomPassParams(getBackend());

    // Set pass state.
    if (m_passStateOverriden)
    {
        // Can use only face culling option.
        auto overridenStateWOCull = m_overridenState & ~RENDER_STATE_FACE_CULL_MASK;
        auto faceCullState = pass->state & RENDER_STATE_FACE_CULL_MASK;
        m_renderBackend->setState(overridenStateWOCull | faceCullState);
    }
    else
    {
        m_renderBackend->setState(pass->state);
    }
}

void RenderManager::render2D()
{
    m_sharedState->setProjectionMatrix(glm::ortho(0.0f, (float)m_viewport.width, (float)m_viewport.height, 0.0f));
    m_sharedState->setViewMatrix(glm::mat4(1.0f));

    // 2D ops pass.
    for (const auto &op : m_renderQueue->rops[RQ_BUCKET_2D])
        drawRenderOperation(op);

    // Draw GUI.
    svc().guiManager().getRenderer()->BeginPaint(m_viewport.width, m_viewport.height);
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

void RenderManager::initialize(const EngineInitParams &params)
{
    m_initParams = params;

    m_renderQueue = make_unique<RenderQueue>();
    m_viewport = { 0, 0, params.windowWidth, params.windowHeight };
    m_renderBackend = CreateRenderBackend(params);
    m_sharedState = IGPUProgramSharedState::create(m_renderBackend->getID());

    m_renderBackend->setClearData({ 0.0f, 0.0f, 0.0f }, 1.0f);

    loadEmbedResources();
}

void RenderManager::shutdown()
{
    m_embedResources.reset();
    m_renderBackend.reset();
}

void RenderManager::drawWorld(World &world)
{
    if (!m_renderBackend)
        return;

    onFrameBegin();

    doRenderWorld(world);

    onFrameEnd();
}

void RenderManager::drawRenderOperation(const RenderOperation &op, RenderPass *passPropsOverride)
{
    if (op.numberOfElements == 0)
    {
        DF3D_ASSERT_MESS(false, "invalid elements count to draw");
        return;
    }

    m_sharedState->setWorldMatrix(op.worldTransform);
    bindPass(passPropsOverride ? passPropsOverride : op.passProps);

    m_renderBackend->bindVertexBuffer(op.vertexBuffer, op.startVertex);
    if (op.indexBuffer.isValid())
        m_renderBackend->bindIndexBuffer(op.indexBuffer);

    m_renderBackend->draw(op.topology, op.numberOfElements);
}

FrameStats RenderManager::getFrameStats() const
{
    if (m_renderBackend)
        return m_renderBackend->getLastFrameStats();
    return {};
}

IRenderBackend& RenderManager::getBackend()
{
    return *m_renderBackend;
}

void RenderManager::destroyEmbedResources()
{
    m_embedResources.reset();
}

void RenderManager::loadEmbedResources()
{
    m_embedResources = make_unique<RenderManagerEmbedResources>(this);
}

void RenderManager::destroyBackend()
{
    m_renderBackend.reset();
}

void RenderManager::createBackend()
{
    m_renderBackend = CreateRenderBackend(m_initParams);
}

RenderBackendID RenderManager::getBackendID()
{
    return m_renderBackend->getID();
}

}
