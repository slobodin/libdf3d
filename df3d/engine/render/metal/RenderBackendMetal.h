#pragma once

#include <cstdint>
#include <df3d_pch.h>
#include <df3d/df3d.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <MetalKit/MetalKit.h>
#include <Metal/Metal.h>

namespace df3d {

class RenderBackendMetal : public IRenderBackend
{
    RenderBackendCaps m_caps;
    FrameStats m_stats;
    
    MTKView *m_mtkView = nullptr;
    
    id<MTLDevice> m_mtlDevice = nullptr;
    id<MTLRenderPipelineState> m_pipelineState = nullptr;
    id<MTLCommandQueue> m_commandQueue;
    
    int m_width;
    int m_height;

public:
    RenderBackendMetal(const EngineInitParams &params);
    ~RenderBackendMetal();

    const RenderBackendCaps& getCaps() const override;
    const FrameStats& getFrameStats() const override;

    void frameBegin() override;
    void frameEnd() override;

    VertexBufferHandle createVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data, GpuBufferUsageType usage) override;
    void destroyVertexBuffer(VertexBufferHandle vbHandle) override;

    void bindVertexBuffer(VertexBufferHandle vbHandle) override;
    void updateVertexBuffer(VertexBufferHandle vbHandle, size_t verticesCount, const void *data) override;

    IndexBufferHandle createIndexBuffer(size_t indicesCount, const void *data, GpuBufferUsageType usage, IndicesType indicesType) override;
    void destroyIndexBuffer(IndexBufferHandle ibHandle) override;

    void bindIndexBuffer(IndexBufferHandle ibHandle) override;
    void updateIndexBuffer(IndexBufferHandle ibHandle, size_t indicesCount, const void *data) override;

    TextureHandle createTexture2D(const TextureInfo &info, uint32_t flags, const void *data) override;
    TextureHandle createCompressedTexture(const TextureResourceData &data, uint32_t flags) override;
    void updateTexture(TextureHandle textureHandle, int w, int h, const void *data) override;
    void destroyTexture(TextureHandle textureHandle) override;

    void bindTexture(TextureHandle textureHandle, int unit) override;

    ShaderHandle createShader(ShaderType type, const char *data) override;

    GpuProgramHandle createGpuProgram(ShaderHandle vertexShaderHandle, ShaderHandle fragmentShaderHandle) override;
    void destroyGpuProgram(GpuProgramHandle programHandle) override;

    FrameBufferHandle createFrameBuffer(TextureHandle *attachments, size_t attachmentCount) override;
    void destroyFrameBuffer(FrameBufferHandle framebufferHandle) override;

    void bindGpuProgram(GpuProgramHandle programHandle) override;
    void requestUniforms(GpuProgramHandle programHandle, std::vector<UniformHandle> &outHandles, std::vector<std::string> &outNames) override;
    void setUniformValue(UniformHandle uniformHandle, const void *data) override;

    void bindFrameBuffer(FrameBufferHandle frameBufferHandle) override;

    void setViewport(int x, int y, int width, int height) override;

    void clearColorBuffer(const glm::vec4 &color) override;
    void clearDepthBuffer() override;
    void clearStencilBuffer() override;
    void enableDepthTest(bool enable) override;
    void enableDepthWrite(bool enable) override;
    void enableScissorTest(bool enable) override;
    void setScissorRegion(int x, int y, int width, int height) override;

    void setBlendingMode(BlendingMode mode) override;
    void setCullFaceMode(FaceCullMode mode) override;

    void draw(Topology type, size_t numberOfElements) override;

    void setDestroyAndroidWorkaround() override { }
};

}
