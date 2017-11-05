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

const size_t MAX_TRANSIENT_BUFFER_SIZE = 0x1000;    // 4k is allowed by Metal
const int MAX_IN_FLIGHT_FRAMES = 3;

class IMetalVertexBuffer
{
    VertexFormat m_format;

public:
    IMetalVertexBuffer(const VertexFormat &format) : m_format(format) { }
    virtual ~IMetalVertexBuffer() = default;

    const VertexFormat& getFormat() const { return m_format; }

    virtual bool initialize(RenderBackendMetal *backend, const void *data, size_t verticesCount) = 0;
    virtual void update(RenderBackendMetal *backend, const void *data, size_t verticesCount) = 0;
    virtual void bind(id <MTLRenderCommandEncoder> encoder, size_t verticesOffset) = 0;
};

class MetalTransientVertexBuffer : public IMetalVertexBuffer
{
    std::array<uint8_t, MAX_TRANSIENT_BUFFER_SIZE> m_transientStorage;
    size_t m_currentSize = 0;

public:
    MetalTransientVertexBuffer(const VertexFormat &format);
    ~MetalTransientVertexBuffer();

    bool initialize(RenderBackendMetal *backend, const void *data, size_t verticesCount) override;
    void update(RenderBackendMetal *backend, const void *data, size_t verticesCount) override;
    void bind(id <MTLRenderCommandEncoder> encoder, size_t verticesOffset) override;
};

class MetalStaticVertexBuffer : public IMetalVertexBuffer
{
    id <MTLBuffer> m_buffer = nil;

public:
    MetalStaticVertexBuffer(const VertexFormat &format);
    ~MetalStaticVertexBuffer();

    bool initialize(RenderBackendMetal *backend, const void *data, size_t verticesCount) override;
    void update(RenderBackendMetal *backend, const void *data, size_t verticesCount) override;
    void bind(id <MTLRenderCommandEncoder> encoder, size_t verticesOffset) override;
};

class MetalDynamicVertexBuffer : public IMetalVertexBuffer
{
    std::array<id<MTLBuffer>, MAX_IN_FLIGHT_FRAMES> m_buffers;
    size_t m_sizeInBytes = 0;
    int m_bufferIdx = 0;

public:
    MetalDynamicVertexBuffer(const VertexFormat &format);
    ~MetalDynamicVertexBuffer();

    bool initialize(RenderBackendMetal *backend, const void *data, size_t verticesCount) override;
    void update(RenderBackendMetal *backend, const void *data, size_t verticesCount) override;
    void bind(id <MTLRenderCommandEncoder> encoder, size_t verticesOffset) override;
    void advanceToTheNextFrame();
};

struct MetalIndexBufferWrapper
{
    MTLIndexType m_indexType;
    id <MTLBuffer> m_buffer = nil;

    bool init(id<MTLDevice> device, const void *data, size_t size);
    void destroy();
};

class MetalTextureWrapper
{
    int m_mipLevel0Width = 0;
    int m_mipLevel0Height = 0;
    PixelFormat m_format;

public:
    id<MTLTexture> m_texture = nil;
    id<MTLSamplerState> m_samplerState = nil;

    bool init(RenderBackendMetal *backend, const TextureResourceData &data, uint32_t flags);
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
    friend struct MetalVertexBufferWrapper;

    dispatch_semaphore_t m_frameBoundarySemaphore;

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

    unique_ptr<IMetalVertexBuffer> m_vertexBuffers[MAX_SIZE];
    MetalIndexBufferWrapper m_indexBuffers[MAX_SIZE];
    MetalTextureWrapper m_textures[MAX_SIZE];
    MetalGpuProgramWrapper m_programs[MAX_SIZE];

    std::vector<MetalDynamicVertexBuffer*> m_dynamicBuffers;

    RenderPassState m_currentPassState;
    VertexBufferHandle m_currentVB;
    IndexBufferHandle m_currentIB;
    GpuProgramHandle m_currentProgram;

    MTKView *m_mtkView = nullptr;
    id<MTLDevice> m_mtlDevice = nullptr;
    id<MTLCommandQueue> m_commandQueue = nullptr;
    id<MTLLibrary> m_defaultLibrary = nullptr;

    id<MTLCommandBuffer> m_commandBuffer = nullptr;
    bool m_firstDrawCall = false;

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

    MTLTextureDescriptor *m_textureDescriptor = nullptr;
    MTLSamplerDescriptor *m_samplerDescriptor = nullptr;
    MTLDepthStencilDescriptor *m_depthStencilDescriptor = nullptr;

    std::unordered_map<uint32_t, id<MTLSamplerState>> m_samplerStateCache;
    std::array<id<MTLDepthStencilState>, 4> m_depthStencilStateCache;

    class RenderPipelinesCache
    {
        RenderBackendMetal *m_backend;

        MTLVertexDescriptor *m_vertexDescriptor = nullptr;
        MTLRenderPipelineDescriptor *m_renderPipelineDescriptor = nullptr;
        std::unordered_map<uint64_t, id <MTLRenderPipelineState>> m_cache;

        uint64_t getHash(GpuProgramHandle program, VertexFormat vf, BlendingMode blending) const;

    public:
        RenderPipelinesCache(RenderBackendMetal *backend);
        ~RenderPipelinesCache();

        id <MTLRenderPipelineState> getOrCreate(GpuProgramHandle program, VertexFormat vf, BlendingMode blending);

        void invalidate();
    };

    unique_ptr<RenderPipelinesCache> m_renderPipelinesCache;

    id<MTLSamplerState> getSamplerState(uint32_t flags);
    id<MTLDepthStencilState> getDepthStencilState(bool depthTestEnabled, bool depthWriteEnabled);

public:
    RenderBackendMetal(const EngineInitParams &params);
    ~RenderBackendMetal();

    const RenderBackendCaps& getCaps() const override;
    const FrameStats& getFrameStats() const override;

    void frameBegin() override;
    void frameEnd() override;

    VertexBufferHandle createVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data) override;
    VertexBufferHandle createDynamicVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data) override;
    void updateDynamicVertexBuffer(VertexBufferHandle vbHandle, size_t verticesCount, const void *data) override;
    void destroyVertexBuffer(VertexBufferHandle vbHandle) override;

    void bindVertexBuffer(VertexBufferHandle vbHandle) override;

    IndexBufferHandle createIndexBuffer(size_t indicesCount, const void *data, IndicesType indicesType) override;
    void destroyIndexBuffer(IndexBufferHandle ibHandle) override;

    void bindIndexBuffer(IndexBufferHandle ibHandle) override;

    TextureHandle createTexture(const TextureResourceData &data, uint32_t flags) override;
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

    void draw(Topology type, size_t numberOfElements, size_t vertexBufferOffset) override;

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

    unique_ptr<IGPUProgramSharedState> createSharedState() override;

    MetalGlobalUniforms* getGlobalUniforms() { return m_uniformBuffer.get(); }
    id<MTLDevice> getMetalDevice() { return m_mtlDevice; }
};

}
