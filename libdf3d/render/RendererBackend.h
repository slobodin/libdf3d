#pragma once

#include "RenderPass.h"
#include "RenderOperation.h"

namespace df3d {

class VertexBuffer;
class IndexBuffer;
class GpuProgramState;
class Viewport;
class Texture2D;
class Light;

class RendererBackend
{
    unique_ptr<GpuProgramState> m_programState;
    shared_ptr<Texture2D> m_whiteTexture;

    int m_maxTextureSize = -1;
    float m_maxAnisotropyLevel = 1.0f;

    void createWhiteTexture();
    void loadResidentGpuPrograms();

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

    bool m_initialized = false;

public:
    RendererBackend();
    ~RendererBackend();

    void loadResources();

    void beginFrame();
    void endFrame();
    void setViewport(const Viewport &viewport);
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

    void clearColorBuffer(const glm::vec4 &color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
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
    void setLight(const Light &light);

    void bindPass(RenderPass *pass);

    void drawVertexBuffer(VertexBuffer *vb, IndexBuffer *ib, RenderOperation::Type type);

    // TODO:
    // Debug draw:
    // Draw AABB
    // Draw Line etc

    int getMaxTextureSize();
    float getMaxAnisotropy();

    void drawOperation(const RenderOperation &op);
};

}
