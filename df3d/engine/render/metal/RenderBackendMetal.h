#pragma once

#include <cstdint>
#include <array>
#include <df3d_pch.h>
#include <df3d/df3d.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <MetalKit/MetalKit.h>
#include <Metal/Metal.h>
#include "MetalGlobalUniforms.h"

namespace df3d {
    
class RenderBackendMetal;

struct MetalBufferWrapper
{
    size_t m_sizeInBytes = 0;
    id <MTLBuffer> m_buffer = nil;

    bool init(id<MTLDevice> device, const void *data, size_t size);
    void destroy();
};

struct MetalVertexBufferWrapper : MetalBufferWrapper
{
    VertexFormat m_vertexFormat;
};

struct MetalIndexBufferWrapper : MetalBufferWrapper
{
    MTLIndexType m_indexType;
};

class MetalTextureWrapper
{    
public:
    TextureInfo m_info;
    id<MTLTexture> m_texture = nil;
    id<MTLSamplerState> m_samplerState = nil;

    bool init(RenderBackendMetal *backend, const TextureInfo &info, uint32_t flags, const void *data);
    void update(int w, int h, const void *data);
    void destroy();
};

class MetalGpuProgramWrapper
{
public:
    enum UniformType
    {
        FLOAT,
        SAMPLER_IDX,
        VEC4
    };

    struct CustomUniform
    {
        void *dataPointer = nullptr;
        UniformType type;
        MetalTextureInputIndex textureKind;
    };

    id<MTLFunction> m_vertexShaderFunction = nil;
    id<MTLFunction> m_fragmentShaderFunction = nil;
    std::vector<CustomUniform> m_customUniforms;

    bool init(RenderBackendMetal *backend, const char *vertexFunctionName, const char *fragmentFunctionName);
    void destroy();
};

class RenderBackendMetal : public IRenderBackend
{
    friend class MetalTextureWrapper;
    friend class MetalGpuProgramWrapper;
    
    RenderBackendCaps m_caps;
    FrameStats m_stats;

    struct RenderPassState
    {
        MTLViewport viewport;
        MTLScissorRect scissorRect;
        MTLCullMode cullMode;
        MTLWinding winding;

        bool depthTestEnabled = true;
        bool depthWriteEnabled = true;
        
        BlendingMode blendingMode = BlendingMode::NONE;

        RenderPassState(int w, int h)
        {
            setViewport(0, 0, w, h);
            resetScissorRect();
            cullMode = MTLCullModeNone;
            winding = MTLWindingCounterClockwise;
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
            viewport.originX = x;
            viewport.originY = y;
            viewport.width = w;
            viewport.height = h;
            viewport.znear = 0.0;
            viewport.zfar = 1.0;
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
    };

    static const int MAX_SIZE = 0xFFF;      // 4k is enough for now.

    HandleBag m_vertexBuffersBag;
    HandleBag m_indexBuffersBag;
    HandleBag m_texturesBag;
    HandleBag m_gpuProgramsBag;

    MetalVertexBufferWrapper m_vertexBuffers[MAX_SIZE];
    MetalIndexBufferWrapper m_indexBuffers[MAX_SIZE];
    MetalTextureWrapper m_textures[MAX_SIZE];
    MetalGpuProgramWrapper m_programs[MAX_SIZE];

    RenderPassState m_currentPassState;
    VertexBufferHandle m_currentVB;
    IndexBufferHandle m_currentIB;
    GpuProgramHandle m_currentProgram;

    MTKView *m_mtkView = nullptr;
    id<MTLDevice> m_mtlDevice = nullptr;
    id<MTLCommandQueue> m_commandQueue = nullptr;
    id<MTLLibrary> m_defaultLibrary = nullptr;

    id<MTLCommandBuffer> m_commandBuffer = nullptr;

    unique_ptr<MetalGlobalUniforms> m_uniformBuffer;

    struct TextureUnit
    {
        TextureHandle textureHandle;
    };

    enum {
        MAX_TEXTURE_UNITS = 8
    };

    TextureUnit m_textureUnits[MAX_TEXTURE_UNITS];

    int m_width = 0;
    int m_height = 0;
    bool m_indexedDrawCall = false;

    void initMetal(const EngineInitParams &params);

    id<MTLRenderPipelineState> createPipeline(VertexFormat vf,
                                              id<MTLFunction> vertexFunction,
                                              id<MTLFunction> fragmentFunction,
                                              BlendingMode blendingMode);
    
    MTLTextureDescriptor *m_textureDescriptor = nullptr;
    MTLSamplerDescriptor *m_samplerDescriptor = nullptr;
    MTLVertexDescriptor *m_vertexDescriptor = nullptr;
    MTLRenderPipelineDescriptor *m_renderPipelineDescriptor = nullptr;
    MTLDepthStencilDescriptor *m_depthStencilDescriptor = nullptr;
    
    std::unordered_map<uint32_t, id<MTLSamplerState>> m_samplerStateCache;
    std::array<id<MTLDepthStencilState>, 4> m_depthStencilStateCache;
    
    id<MTLSamplerState> getSamplerState(uint32_t flags);
    id<MTLDepthStencilState> getDepthStencilState(bool depthTestEnabled, bool depthWriteEnabled);

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

    IndexBufferHandle createIndexBuffer(size_t indicesCount, const void *data, GpuBufferUsageType usage, IndicesType indicesType) override;
    void destroyIndexBuffer(IndexBufferHandle ibHandle) override;

    void bindIndexBuffer(IndexBufferHandle ibHandle) override;

    TextureHandle createTexture2D(const TextureInfo &info, uint32_t flags, const void *data) override;
    TextureHandle createCompressedTexture(const TextureResourceData &data, uint32_t flags) override;
    void updateTexture(TextureHandle textureHandle, int w, int h, const void *data) override;
    void destroyTexture(TextureHandle textureHandle) override;

    void bindTexture(TextureHandle textureHandle, int unit) override;

    ShaderHandle createShader(ShaderType type, const char *data) override
    {
        DF3D_ASSERT(false);
        return {};
    }

    GpuProgramHandle createGpuProgram(ShaderHandle vertexShaderHandle, ShaderHandle fragmentShaderHandle) override
    {
        DF3D_ASSERT(false);
        return {};
    }

    GpuProgramHandle createGpuProgramMetal(const char *vertexFunctionName, const char *fragmentFunctionName) override;
    void destroyGpuProgram(GpuProgramHandle programHandle) override;

    void bindGpuProgram(GpuProgramHandle programHandle) override;

    void requestUniforms(GpuProgramHandle programHandle, std::vector<UniformHandle> &outHandles, std::vector<std::string> &outNames) override;
    void setUniformValue(GpuProgramHandle programHandle, UniformHandle uniformHandle, const void *data) override;

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
    RenderBackendID getID() const { return RenderBackendID::METAL; }

    FrameBufferHandle createFrameBuffer(TextureHandle *attachments, size_t attachmentCount) override
    {
        DF3D_ASSERT_MESS(false, "RenderBackendMetal::createFrameBuffer is not implemented");
        return {};
    }

    void destroyFrameBuffer(FrameBufferHandle framebufferHandle) override
    {
        DF3D_ASSERT_MESS(false, "RenderBackendMetal::destroyFrameBuffer is not implemented");
    }

    void bindFrameBuffer(FrameBufferHandle frameBufferHandle) override
    {
        DF3D_ASSERT_MESS(false, "RenderBackendMetal::bindFrameBuffer is not implemented");
    }

    MetalGlobalUniforms* getGlobalUniforms() { return m_uniformBuffer.get(); }
};

}
