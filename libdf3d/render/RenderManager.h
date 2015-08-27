#pragma once

#include "RenderOperation.h"
#include "RenderStats.h"
#include "RenderingCapabilities.h"

FWD_MODULE_CLASS(scene, Scene)
FWD_MODULE_CLASS(scene, Node)
FWD_MODULE_CLASS(scene, Camera)
FWD_MODULE_CLASS(base, EngineController)

namespace df3d { namespace render {

class RendererBackend;
class VertexBuffer;
class GpuProgram;
class RenderTarget;
class RenderTargetScreen;
class RenderTargetTexture;
class Viewport;
class Material;
class RenderQueue;

struct RenderManagerInitParams
{
    int viewportWidth = DEFAULT_WINDOW_WIDTH;
    int viewportHeight = DEFAULT_WINDOW_HEIGHT;

    RenderingCapabilities renderingCaps = RenderingCapabilities::getDefaults();
};

// Forward renderer.
class RenderManager
{
    static const size_t MAX_POSPROCESS_PASSES = 4;

    friend class base::EngineController;

    unique_ptr<RendererBackend> m_renderer;
    RenderingCapabilities m_renderingCaps;

    // Ambient pass support.
    shared_ptr<RenderPass> m_ambientPassProps;

    // For postfx support.
    shared_ptr<RenderTargetScreen> m_screenRt;
    shared_ptr<RenderTargetTexture> m_textureRt;
    shared_ptr<Material> m_defaultPostProcessMaterial;
    shared_ptr<VertexBuffer> m_quadVb;
    // FIXME: oes2.0 doesn't support mrt's.
    shared_ptr<RenderTargetTexture> m_postProcessPassBuffers[MAX_POSPROCESS_PASSES];

    unique_ptr<RenderQueue> m_renderQueue;

    void createQuadRenderOperation();
    void createRenderTargets(const Viewport &vp);
    void createAmbientPassProps();

    void debugDrawPass();
    void postProcessPass(shared_ptr<Material> material);

    RenderStats m_stats;
    RenderStats m_lastStats;

    RenderManager(RenderManagerInitParams params);
    ~RenderManager();

public:
    void update(shared_ptr<scene::Scene> renderableScene);
    void drawScene(shared_ptr<scene::Scene> sc);
    void drawOperation(const RenderOperation &op);
    void drawGUI();

    void onFrameBegin();
    void onFrameEnd();

    const RenderStats &getLastRenderStats() const;
    shared_ptr<RenderTargetScreen> getScreenRenderTarget() const;

    const RenderingCapabilities &getRenderingCapabilities() const;

    RendererBackend *getRenderer() const;
};

} }
