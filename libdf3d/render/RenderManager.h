#pragma once

#include "RenderOperation.h"
#include "RenderStats.h"

#include "Renderer.h"

FWD_MODULE_CLASS(scene, Scene)
FWD_MODULE_CLASS(scene, Node)
FWD_MODULE_CLASS(scene, Camera)
FWD_MODULE_CLASS(base, Controller)

namespace df3d { namespace render {

class Renderer;
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
    bool debugDraw = false;

    // fullscreen, etc
    // rendering API
};

// Forward renderer.
class RenderManager
{
    static const size_t MAX_POSPROCESS_PASSES = 4;

    friend class base::Controller;

    unique_ptr<Renderer> m_renderer;

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

    bool m_debugDrawEnabled = false;

    RenderManager(RenderManagerInitParams params);
    ~RenderManager();

public:
    void enableDebugDraw(bool enable);
    bool isDebugDrawEnabled();

    void update(shared_ptr<scene::Scene> renderableScene);
    void drawScene(shared_ptr<scene::Scene> sc);
    void drawOperation(const RenderOperation &op);
    void drawGUI();

    void onFrameBegin();
    void onFrameEnd();

    const RenderStats &getLastRenderStats() const;
    shared_ptr<RenderTargetScreen> getScreenRenderTarget() const;

    Renderer *getRenderer() const;
};

} }