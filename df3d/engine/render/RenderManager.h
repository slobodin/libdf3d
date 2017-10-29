#pragma once

#include "RenderCommon.h"
#include "Viewport.h"
#include <df3d/engine/EngineInitParams.h>

namespace df3d {

class IRenderBackend;
class World;
class RenderOperation;
class GpuProgramSharedState;
class RenderPass;
struct GpuProgramResource;
struct RenderQueue;
class RenderManager;
struct EngineInitParams;

struct RenderManagerEmbedResources : private NonCopyable
{
    TextureHandle whiteTexture;
    GpuProgramResource *coloredProgram;
    GpuProgramResource *ambientPassProgram;
    RenderPass *ambientPass;

    RenderManagerEmbedResources(RenderManager *render);
    ~RenderManagerEmbedResources();
};

class RenderManager : NonCopyable
{
    friend class EngineController;
    
    EngineInitParams m_initParams;

    unique_ptr<RenderQueue> m_renderQueue;
    unique_ptr<IRenderBackend> m_renderBackend;
    unique_ptr<GpuProgramSharedState> m_sharedState;

    Viewport m_viewport;

    // Forward rendering stuff.
    bool m_blendModeOverriden = false;
    bool m_depthTestOverriden = false;
    bool m_depthWriteOverriden = false;
    bool m_initialized = false;
    int m_width = 0;
    int m_height = 0;
    unique_ptr<RenderManagerEmbedResources> m_embedResources;

    void onFrameBegin();
    void onFrameEnd();
    void doRenderWorld(World &world);

    void bindPass(RenderPass *pass);

    void render2D();

public:
    RenderManager();
    ~RenderManager();

    void initialize(const EngineInitParams &params);
    void shutdown();

    void drawWorld(World &world);
    void drawRenderOperation(const RenderOperation &op);

    const Viewport& getViewport() const;
    FrameStats getFrameStats() const;
    IRenderBackend& getBackend();
    const RenderManagerEmbedResources& getEmbedResources() const { return *m_embedResources; }

    void destroyEmbedResources();
    void loadEmbedResources();
    void destroyBackend();
    void createBackend();
};

}
