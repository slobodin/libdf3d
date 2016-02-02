#pragma once

#include "RenderStats.h"
#include "RenderCommon.h"
#include <libdf3d/base/EngineInitParams.h>

namespace df3d {

class RendererBackend;
class VertexBuffer;
class GpuProgram;
class RenderTarget;
class RenderTargetScreen;
class RenderTargetTexture;
class Viewport;
class Material;
class RenderQueue;
class RenderPass;
class RenderOperation;
class Camera;
class World;

// Forward renderer.
class RenderManager : utils::NonCopyable
{
    friend class EngineController;

    static const size_t MAX_POSPROCESS_PASSES = 4;

    unique_ptr<RendererBackend> m_renderer;
    EngineInitParams m_initParams;

    // Ambient pass support.
    unique_ptr<RenderPass> m_ambientPassProps;

    // For postfx support.
    unique_ptr<RenderTargetScreen> m_screenRt;
    unique_ptr<RenderTargetTexture> m_textureRt;
    unique_ptr<Material> m_defaultPostProcessMaterial;
    shared_ptr<VertexBuffer> m_quadVb;
    // FIXME: oes2.0 doesn't support mrt's.
    unique_ptr<RenderTargetTexture> m_postProcessPassBuffers[MAX_POSPROCESS_PASSES];

    unique_ptr<RenderQueue> m_renderQueue;

    void createQuadRenderOperation();
    void createRenderTargets(const Viewport &vp);
    void createAmbientPassProps();

    void postProcessPass(shared_ptr<Material> material);

    RenderStats m_stats;
    RenderStats m_lastStats;

    void loadEmbedResources();
    void onFrameBegin();
    void onFrameEnd();
    void doRenderWorld(World &world);

public:
    RenderManager(EngineInitParams params);
    ~RenderManager();

    void drawWorld(World &world);

    const RenderStats& getLastRenderStats() const;
    const RenderTargetScreen& getScreenRenderTarget() const;

    const RenderingCapabilities& getRenderingCapabilities() const;

    RendererBackend* getRenderer();
};

}
