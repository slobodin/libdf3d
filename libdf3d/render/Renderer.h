#pragma once

#include "RenderStats.h"
#include "RenderPass.h"
#include "RenderOperation.h"

FWD_MODULE_CLASS(components, LightComponent)

namespace df3d { namespace render {

class VertexBuffer;
class IndexBuffer;
class GpuProgramState;

class Renderer
{
    RenderStats *m_renderStats = nullptr;

    scoped_ptr<GpuProgramState> m_programState;
    shared_ptr<Texture> m_whiteTexture;

    void loadEmbedGPUPrograms() const;
    void createWhiteTexture();

    // Helpers.
    void setBlendMode(RenderPass::BlendingMode bm);
    void setFrontFace(RenderPass::WindingOrder wo);
    void setCullFace(RenderPass::FaceCullMode cm);
    void setPolygonDrawMode(RenderPass::PolygonMode pm);
    void updateProgramUniformValues(GpuProgram *program, RenderPass *pass);
    // Updates texture samplers for current pass.
    void updateTextureSamplers();

    bool m_blendModeOverriden = false;
    bool m_frontFaceOverriden = false;
    bool m_cullFaceOverriden = false;
    bool m_polygonDrawModeOverriden = false;
    bool m_depthTestOverriden = false;
    bool m_depthWriteOverriden = false;

public:
    Renderer();
    ~Renderer();

    bool initialize();
    void shutdown();

    void setRenderStatsLocation(RenderStats *renderStats);

    void beginFrame();
    void endFrame();
    void setViewport(unsigned int width, unsigned int height);
    void enableDepthTest(bool enable);
    void enableDepthWrite(bool enable);
    void enableScissorTest(bool enable);
    void setScissorRegion(int x, int y, int width, int height);
    void enableFog(float density, const glm::vec3 &fogColor);

    void enableBlendModeOverride(RenderPass::BlendingMode bm);
    void enableFrontFaceOverride(RenderPass::WindingOrder wo);
    void enableCullFaceOverride(RenderPass::FaceCullMode cm);
    void enablePolygonDrawModeOverride(RenderPass::PolygonMode pm);
    void enableDepthTestOverride(bool enable);
    void enableDepthWriteOverride(bool enable);

    void disableBlendModeOverride() { m_blendModeOverriden = false; }
    void disableFrontFaceOverride() { m_frontFaceOverriden = false; }
    void disableCullFaceOverride() { m_cullFaceOverriden = false; }
    void disablePolygonDrawModeOverride() { m_polygonDrawModeOverriden = false; }
    void disableDepthTestOverride() { m_depthTestOverriden = false; }
    void disableDepthWriteOverride() { m_depthWriteOverriden = false; }

    void clearColorBuffer(const glm::vec3 &color = glm::vec3());
    void clearDepthBuffer();
    void clearStencilBuffer();

    // TODO:
    // do not use glReadPixels.
    // TODO:
    // Buffer ids.
    float readDepthBuffer(int x, int y);

    void setWorldMatrix(const glm::mat4 &worldm);
    void setCameraMatrix(const glm::mat4 &viewm);
    void setProjectionMatrix(const glm::mat4 &projm);

    void setAmbientLight(const glm::vec3 &ambient);
    void setAmbientLight(float ra, float ga, float ba);
    void setLight(const components::LightComponent *light);

    void bindPass(shared_ptr<RenderPass> pass);

    // bind render target

    void drawVertexBuffer(shared_ptr<VertexBuffer> vb, shared_ptr<IndexBuffer> ib, RenderOperation::Type type);
    //virtual void drawVertexBuffer(shared_ptr<VertexBuffer> vb, shared_ptr<RenderTargetTexture> rtt) = 0;

    // TODO:
    // Debug draw:
    // Draw AABB
    // Draw Line etc
};

} }