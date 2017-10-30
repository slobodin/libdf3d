#pragma once

#include <cstdint>
#include <df3d_pch.h>
#include <df3d/df3d.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <MetalKit/MetalKit.h>
#include <Metal/Metal.h>

namespace df3d {

struct MetalBufferWrapper
{
    size_t m_sizeInBytes = 0;
    id <MTLBuffer> m_buffer = nil;

    bool init(id<MTLDevice> device, const void *data, size_t size);
    void destroy();
};
    
struct MetalShaderWrapper
{
    ShaderType m_type = ShaderType::UNDEFINED;
    id<MTLFunction> m_function = nil;
    
    bool init(id<MTLDevice> device, const char *source, ShaderType shaderType);
    void destroy();
};
    
struct MetalGpuProgramWrapper
{
    MetalShaderWrapper *m_vertexShader = nullptr;
    MetalShaderWrapper *m_fragmentShader = nullptr;
    
    ShaderHandle m_vShaderHandle;
    ShaderHandle m_fShaderHandle;
    
    bool init(id<MTLDevice> device, MetalShaderWrapper *vShader, MetalShaderWrapper *fShader);
    void destroy();
};

class RenderBackendMetal : public IRenderBackend
{
    struct RenderPassState
    {
        MTLViewport viewport;
        MTLScissorRect scissorRect;
        MTLCullMode cullMode;
        MTLWinding winding;
        
        bool depthTestEnabled = true;
        bool depthWriteEnabled = true;
        
        RenderPassState(int w, int h)
        {
            setViewport(0, 0, w, h);
            resetScissorRect();
            cullMode = MTLCullModeNone;
            winding = MTLWindingClockwise;
        }
        
        void setCullMode(FaceCullMode mode)
        {
            switch (mode)
            {
            case FaceCullMode::NONE:
                cullMode = MTLCullModeNone;
                break;
            case FaceCullMode::FRONT:
                cullMode = MTLCullModeFront;
                break;
            case FaceCullMode::BACK:
                cullMode = MTLCullModeBack;
                break;
            default:
                DF3D_ASSERT(false);
                break;
            }
        }
        
        void setViewport(int x, int y, int w, int h)
        {
            viewport = (MTLViewport){ (double)x, (double)y, (double)w, (double)h, -1.0, 1.0 };
        }
        
        void setScissorRect(int x, int y, int w, int h)
        {
            scissorRect = (MTLScissorRect){
                static_cast<NSUInteger>(x),
                static_cast<NSUInteger>(y),
                static_cast<NSUInteger>(w),
                static_cast<NSUInteger>(h) };
        }
        
        void resetScissorRect()
        {
            setScissorRect(0, 0, (int)viewport.width, (int)viewport.height);
        }
        
        id <MTLRenderCommandEncoder> createEncoder(id<MTLCommandBuffer> commandBuffer, MTLRenderPassDescriptor *renderPassDescriptor);
    };
    
    static const int MAX_SIZE = 0xFFF;      // 4k is enough for now.

    HandleBag m_vertexBuffersBag;
    HandleBag m_indexBuffersBag;
    HandleBag m_shadersBag;
    HandleBag m_gpuProgramsBag;

    MetalBufferWrapper m_vertexBuffers[MAX_SIZE];
    MetalBufferWrapper m_indexBuffers[MAX_SIZE];
    MetalShaderWrapper m_shaders[MAX_SIZE];
    MetalGpuProgramWrapper m_programs[MAX_SIZE];

    RenderBackendCaps m_caps;
    FrameStats m_stats;
    
    RenderPassState m_currentPassState;
    id <MTLBuffer> m_currentVertexBuffer = nil;
    id <MTLBuffer> m_currentIndexBuffer = nil;

    MTKView *m_mtkView = nullptr;
    id<MTLDevice> m_mtlDevice = nullptr;
    id<MTLCommandQueue> m_commandQueue = nullptr;
    
    id<MTLCommandBuffer> m_currentCommandBuffer = nullptr;

    int m_width;
    int m_height;

    void initMetal(const EngineInitParams &params);
    
    void destroyShader(ShaderHandle shaderHandle);

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
