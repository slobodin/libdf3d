#pragma once

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

    unique_ptr<RendererBackend> m_renderer;
    EngineInitParams m_initParams;

    // Ambient pass support.
    unique_ptr<RenderPass> m_ambientPassProps;

    unique_ptr<RenderTargetScreen> m_screenRt;

    // For postfx support.
    unique_ptr<RenderTargetTexture> m_textureRt;
    shared_ptr<VertexBuffer> m_quadVb;
    unique_ptr<RenderTargetTexture> m_postProcessPassBuffers[2];

    unique_ptr<RenderQueue> m_renderQueue;

    void createQuadRenderOperation();
    void createRenderTargets(const Viewport &vp);
    void createAmbientPassProps();

    void postProcessPass(shared_ptr<Material> material);

    void loadEmbedResources();
    void onFrameBegin();
    void onFrameEnd();
    void doRenderWorld(World &world);

public:
    RenderManager(EngineInitParams params);
    ~RenderManager();

    void drawWorld(World &world);

    const RenderTargetScreen& getScreenRenderTarget() const;
    const RenderingCapabilities& getRenderingCapabilities() const;

    RendererBackend* getRenderer();
};

}
