#pragma once

#include "RenderCommon.h"
#include "Viewport.h"
#include <libdf3d/base/EngineInitParams.h>

namespace df3d {

class IRenderBackend;
class Viewport;
class Material;
class RenderQueue;
class RenderPass;
class World;

// Forward renderer.
class DF3D_DLL RenderManager : utils::NonCopyable
{
    friend class EngineController;

    EngineInitParams m_initParams;
    unique_ptr<RenderQueue> m_renderQueue;

    Viewport m_viewport;

    // Ambient pass support.
    unique_ptr<RenderPass> m_ambientPassProps;

    void createQuadRenderOperation();
    void createAmbientPassProps();

    void loadEmbedResources();
    void onFrameBegin();
    void onFrameEnd();
    void doRenderWorld(World &world);

public:
    RenderManager(EngineInitParams params);
    ~RenderManager();

    void drawWorld(World &world);

    const Viewport& getViewport() const;
    const RenderingCapabilities& getRenderingCapabilities() const;

    IRenderBackend& getBackend();
};

}
