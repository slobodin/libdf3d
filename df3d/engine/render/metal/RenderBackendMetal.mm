#include <df3d/df3d.h>
#include "RenderBackendMetal.h"

namespace df3d {

    namespace {
        
        struct GPUMemStats
        {
            int totalMem = 0;
        
            void addAllocation(uint32_t bytes)
            {
                totalMem += bytes;
            }
            
            void removeAllocation(uint32_t bytes)
            {
                totalMem -= bytes;
                if (totalMem < 0)
                {
                    DF3D_ASSERT(false);
                    totalMem = 0;
                }
            }
        };
        
#ifdef _DEBUG
        GPUMemStats g_stats;
#endif

        const int MAX_TRANSIENT_BUFFER_SIZE = 0x1000;    // 4k is allowed by Metal
        const int MAX_IN_FLIGHT_FRAMES = 3;

        MTLBlendFactor g_blendFuncLookup[] = {
            MTLBlendFactorZero, // INVALID FACTOR

            MTLBlendFactorOne,                  // RENDER_STATE_BLEND_ONE
            MTLBlendFactorSourceAlpha,          // RENDER_STATE_BLEND_SRC_ALPHA
            MTLBlendFactorOneMinusSourceAlpha   // RENDER_STATE_BLEND_ONE_MINUS_SRC_ALPHA
        };

        int GetMipMapLevelCount(int width, int height)
        {
            return (int)floor(std::log2((double)std::max(width, height))) + 1;
        }

        void SetupTextureFiltering(MTLSamplerDescriptor *descriptor, uint32_t flags)
        {
            const auto filtering = flags & TEXTURE_FILTERING_MASK;

            if (filtering == TEXTURE_FILTERING_NEAREST)
            {
                descriptor.minFilter = MTLSamplerMinMagFilterNearest;
                descriptor.magFilter = MTLSamplerMinMagFilterNearest;
                descriptor.mipFilter = MTLSamplerMipFilterNotMipmapped;
            }
            else if (filtering == TEXTURE_FILTERING_BILINEAR)
            {
                descriptor.minFilter = MTLSamplerMinMagFilterLinear;
                descriptor.magFilter = MTLSamplerMinMagFilterLinear;
                descriptor.mipFilter = MTLSamplerMipFilterNotMipmapped;
            }
            else if (filtering == TEXTURE_FILTERING_TRILINEAR ||
                     filtering == TEXTURE_FILTERING_ANISOTROPIC)
            {
                descriptor.minFilter = MTLSamplerMinMagFilterLinear;
                descriptor.magFilter = MTLSamplerMinMagFilterLinear;
                descriptor.mipFilter = MTLSamplerMipFilterLinear;
            }
            else
                DFLOG_WARN("SetupTextureFiltering failed");
        }

        MTLSamplerAddressMode GetSamplerAddressMode(uint32_t flags)
        {
            const auto mode = flags & TEXTURE_WRAP_MODE_MASK;

            if (mode == TEXTURE_WRAP_MODE_CLAMP)
                return MTLSamplerAddressModeClampToEdge;
            else if (mode == TEXTURE_WRAP_MODE_REPEAT)
                return MTLSamplerAddressModeRepeat;

            DFLOG_WARN("GetSamplerAddressMode was set to default: MTLSamplerAddressModeRepeat");

            return MTLSamplerAddressModeRepeat;
        }

        void SetupWrapMode(MTLSamplerDescriptor *descriptor, uint32_t flags)
        {
            auto samplerAddessMode = GetSamplerAddressMode(flags);
            descriptor.sAddressMode = samplerAddessMode;
            descriptor.tAddressMode = samplerAddessMode;
            descriptor.rAddressMode = samplerAddessMode;
        }

        int GetBPPForFormat(PixelFormat format)
        {
            switch (format) {
                case PixelFormat::RGBA:
                    return 4;
                default:
                    break;
            }
            DF3D_ASSERT(false);
            return 0;
        }

        MTLPixelFormat GetTextureFormat(PixelFormat pixelFormat)
        {
            switch (pixelFormat) {
                case PixelFormat::RGBA:
                    return MTLPixelFormatRGBA8Unorm;
                case PixelFormat::KTX:
                    return MTLPixelFormatPVRTC_RGB_4BPP;
                case PixelFormat::RGB:
                    // Not supported!
                    break;
                default:
                    break;
            }
            DF3D_ASSERT(false);
            return MTLPixelFormatInvalid;
        }

        MTLVertexFormat GetVertexFormatForComponentsCount(size_t count)
        {
            switch (count)
            {
                case 1:
                    return MTLVertexFormatFloat;
                case 2:
                    return MTLVertexFormatFloat2;
                case 3:
                    return MTLVertexFormatFloat3;
                case 4:
                    return MTLVertexFormatFloat4;
                default:
                    break;
            }
            DF3D_ASSERT(false);
            return MTLVertexFormatInvalid;
        }

        MTLPrimitiveType GetPrimitiveType(Topology topology)
        {
            switch (topology)
            {
                case Topology::LINES:
                    return MTLPrimitiveTypeLine;
                case Topology::TRIANGLES:
                    return MTLPrimitiveTypeTriangle;
                case Topology::LINE_STRIP:
                    return MTLPrimitiveTypeLineStrip;
                case Topology::TRIANGLE_STRIP:
                    return MTLPrimitiveTypeTriangleStrip;
                default:
                    break;
            }
            DF3D_ASSERT(false);
            return MTLPrimitiveTypeTriangle;
        }
    }

class SamplerStateCache
{
    std::unordered_map<uint32_t, id<MTLSamplerState>> m_data;
    MTLSamplerDescriptor *m_samplerDescriptor = nullptr;
    id<MTLDevice> m_device;

public:
    SamplerStateCache(id<MTLDevice> device)
    {
        m_samplerDescriptor = [MTLSamplerDescriptor new];
        m_device = device;
    }

    ~SamplerStateCache()
    {
        for (const auto &kv : m_data)
            [kv.second release];

        [m_samplerDescriptor release];
    }

    id<MTLSamplerState> getOrCreate(uint32_t flags)
    {
        auto found = m_data.find(flags);
        if (found == m_data.end())
        {
            SetupWrapMode(m_samplerDescriptor, flags);
            SetupTextureFiltering(m_samplerDescriptor, flags);

            m_samplerDescriptor.lodMinClamp = 0;
            m_samplerDescriptor.lodMaxClamp = FLT_MAX;
            m_samplerDescriptor.normalizedCoordinates = TRUE;

            if ((flags & TEXTURE_FILTERING_MASK) == TEXTURE_FILTERING_ANISOTROPIC)
                m_samplerDescriptor.maxAnisotropy = 16;
            else
                m_samplerDescriptor.maxAnisotropy = 1;

            auto newState = [m_device newSamplerStateWithDescriptor: m_samplerDescriptor];

            m_data[flags] = newState;

            return newState;
        }

        return found->second;
    }
};

class DepthStencilStateCache
{
    std::array<id<MTLDepthStencilState>, 4> m_data;
    MTLDepthStencilDescriptor *m_depthStencilDescriptor = nullptr;
    id<MTLDevice> m_device;

public:
    DepthStencilStateCache(id<MTLDevice> device)
    {
        m_depthStencilDescriptor = [MTLDepthStencilDescriptor new];
        for (auto &item : m_data)
            item = nil;
        m_device = device;
    }

    ~DepthStencilStateCache()
    {
        for (auto item : m_data)
            [item release];

        [m_depthStencilDescriptor release];
    }

    id<MTLDepthStencilState> getOrCreate(uint64_t state)
    {
        bool depthTestEnabled = false;
        if (state & RENDER_STATE_DEPTH_MASK)
            depthTestEnabled = true;

        bool depthWriteEnabled = false;
        if ((state & RENDER_STATE_DEPTH_WRITE_MASK) == RENDER_STATE_DEPTH_WRITE)
            depthWriteEnabled = true;

        // FIXME: implement RENDER_STATE_DEPTH_MASK.
        int hash = ((int)depthTestEnabled << 1) | (int)depthWriteEnabled;
        DF3D_ASSERT(hash >= 0 && hash < m_data.size());

        auto item = m_data[hash];
        if (item == nil)
        {
            m_depthStencilDescriptor.depthCompareFunction = depthTestEnabled ? MTLCompareFunctionLessEqual : MTLCompareFunctionAlways;
            m_depthStencilDescriptor.depthWriteEnabled = depthWriteEnabled;

            item = [m_device newDepthStencilStateWithDescriptor:m_depthStencilDescriptor];

            m_data[hash] = item;
        }

        return item;
    }
};

class RenderPipelinesCache
{
    std::unordered_map<uint64_t, id <MTLRenderPipelineState>> m_data;

    MTLRenderPipelineDescriptor *m_renderPipelineDescriptor = nullptr;
    MTLVertexDescriptor *m_vertexDescriptor = nullptr;

    uint64_t getHash(GPUProgramHandle program, VertexFormat vf, uint64_t renderState) const
    {
        uint64_t blending = renderState & RENDER_STATE_BLENDING_MASK;

        uint64_t res = (uint64_t)program.getID() << 32;
        uint32_t h1 = (uint32_t)vf.getHash() << 16;
        uint32_t h2 = (uint32_t)blending;

        return res | h1 | h2;
    }

public:
    RenderPipelinesCache()
    {
        m_renderPipelineDescriptor = [MTLRenderPipelineDescriptor new];
        m_vertexDescriptor = [MTLVertexDescriptor new];
    }

    ~RenderPipelinesCache()
    {
        invalidate();

        [m_renderPipelineDescriptor release];
        [m_vertexDescriptor release];
    }

    id <MTLRenderPipelineState> getOrCreate(id<MTLDevice> device, id<MTLFunction> vertexFn, id<MTLFunction> fragmentFn,
                                            MTLPixelFormat colorFormat, MTLPixelFormat depthFormat,
                                            VertexFormat vf, uint64_t renderState,
                                            GPUProgramHandle programHandle)
    {
        auto hash = getHash(programHandle, vf, renderState);
        auto found = m_data.find(hash);
        if (found == m_data.end())
        {
            [m_vertexDescriptor reset];

            for (uint16_t i = VertexFormat::POSITION; i != VertexFormat::COUNT; i++)
            {
                auto attrib = (VertexFormat::VertexAttribute)i;

                if (!vf.hasAttribute(attrib))
                    continue;

                size_t offset = vf.getOffsetTo(attrib);
                size_t count = vf.getCompCount(attrib);

                m_vertexDescriptor.attributes[i].format = GetVertexFormatForComponentsCount(count);
                m_vertexDescriptor.attributes[i].bufferIndex = 0;
                m_vertexDescriptor.attributes[i].offset = offset;
            }

            m_vertexDescriptor.layouts[0].stride = vf.getVertexSize();
            m_vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;

            [m_renderPipelineDescriptor reset];

            m_renderPipelineDescriptor.vertexFunction = vertexFn;
            m_renderPipelineDescriptor.fragmentFunction = fragmentFn;
            m_renderPipelineDescriptor.vertexDescriptor = m_vertexDescriptor;
            m_renderPipelineDescriptor.colorAttachments[0].pixelFormat = colorFormat;
            m_renderPipelineDescriptor.depthAttachmentPixelFormat = depthFormat;

            auto colorAttachment = m_renderPipelineDescriptor.colorAttachments[0];

            auto srcFactor = GetBlendingSrcFactor(renderState);
            auto dstFactor = GetBlendingDstFactor(renderState);

            if (srcFactor == 0 && dstFactor == 0)
            {
                colorAttachment.blendingEnabled = false;
            }
            else
            {
                colorAttachment.blendingEnabled = true;

                DF3D_ASSERT(srcFactor >= RENDER_STATE_BLEND_ONE && srcFactor <= RENDER_STATE_BLEND_ONE_MINUS_SRC_ALPHA);
                DF3D_ASSERT(dstFactor >= RENDER_STATE_BLEND_ONE && dstFactor <= RENDER_STATE_BLEND_ONE_MINUS_SRC_ALPHA);

                colorAttachment.sourceRGBBlendFactor = colorAttachment.sourceAlphaBlendFactor = g_blendFuncLookup[srcFactor];
                colorAttachment.destinationRGBBlendFactor = colorAttachment.destinationAlphaBlendFactor = g_blendFuncLookup[dstFactor];
            }

            NSError *error = nil;
            auto pipeline = [device newRenderPipelineStateWithDescriptor:m_renderPipelineDescriptor error:&error];
            if (pipeline == nil)
            {
                DFLOG_WARN("Failed to create pipeline: %s", error.localizedDescription.UTF8String);
                return nil;
            }

            m_data[hash] = pipeline;
            return pipeline;
        }

        return found->second;
    }

    void invalidate()
    {
        for (const auto &item : m_data)
            [item.second release];
        m_data.clear();
    }
};

class MetalVertexBuffer
{
    VertexFormat m_format;

public:
    MetalVertexBuffer(const VertexFormat &format) : m_format(format) { }
    virtual ~MetalVertexBuffer() = default;

    const VertexFormat& getFormat() const { return m_format; }

    virtual bool initialize(id<MTLDevice> device, const void *data, uint32_t bufferSize) = 0;
    virtual void updateWithData(const void *data, uint32_t offset, uint32_t length) = 0;
    virtual void bindBuffer(id<MTLRenderCommandEncoder> encoder, uint32_t offset) = 0;
    virtual void advanceToTheNextFrame() { }
    virtual uint32_t getSize() const = 0;
};

class MetalTransientVertexBuffer : public MetalVertexBuffer
{
    std::array<uint8_t, MAX_TRANSIENT_BUFFER_SIZE> m_transientStorage;
    uint32_t m_bufferSize = 0;

public:
    MetalTransientVertexBuffer(const VertexFormat &format)
        : MetalVertexBuffer(format)
    {

    }

    bool initialize(id<MTLDevice> device, const void *data, uint32_t bufferSize) override
    {
        DF3D_ASSERT(bufferSize <= m_transientStorage.size());

        m_bufferSize = bufferSize;

        if (data != nullptr)
            updateWithData(data, 0, m_bufferSize);

        return true;
    }

    void updateWithData(const void *data, uint32_t offset, uint32_t length) override
    {
        DF3D_ASSERT(offset + length <= m_bufferSize);
        DF3D_ASSERT(data != nullptr);

        memcpy(m_transientStorage.data() + offset, data, length);
    }

    void bindBuffer(id<MTLRenderCommandEncoder> encoder, uint32_t offset) override
    {
        DF3D_ASSERT(offset < m_bufferSize);
        auto lenUpdate = m_bufferSize - offset;

        [encoder setVertexBytes:m_transientStorage.data() + offset length:lenUpdate atIndex:0];
    }
    
    uint32_t getSize() const override { return m_transientStorage.size(); }
};

class MetalStaticVertexBuffer : public MetalVertexBuffer
{
    id <MTLBuffer> m_buffer = nil;

public:
    MetalStaticVertexBuffer(const VertexFormat &format)
        : MetalVertexBuffer(format)
    {

    }

    ~MetalStaticVertexBuffer()
    {
        [m_buffer release];
    }

    bool initialize(id<MTLDevice> device, const void *data, uint32_t bufferSize) override
    {
        DF3D_ASSERT(data != nullptr);
        m_buffer = [device newBufferWithBytes:data
                                       length:bufferSize
                                      options:MTLResourceStorageModeShared];

        return m_buffer != nil;
    }

    void updateWithData(const void *data, uint32_t offset, uint32_t length) override
    {
        DF3D_ASSERT_MESS(false, "Not supported!");
    }

    void bindBuffer(id<MTLRenderCommandEncoder> encoder, uint32_t offset) override
    {
        [encoder setVertexBuffer:m_buffer offset:offset atIndex:0];
    }
    
    uint32_t getSize() const override
    {
        return m_buffer.length;
    }
};

class MetalDynamicVertexBuffer : public MetalVertexBuffer
{
    std::array<id<MTLBuffer>, MAX_IN_FLIGHT_FRAMES> m_buffers;
    uint32_t m_bufferSize = 0;
    int m_bufferIdx = 0;

public:
    MetalDynamicVertexBuffer(const VertexFormat &format)
        : MetalVertexBuffer(format)
    {

    }
    ~MetalDynamicVertexBuffer()
    {
        for (int i = 0; i < MAX_IN_FLIGHT_FRAMES; i++)
            [m_buffers[i] release];
    }

    bool initialize(id<MTLDevice> device, const void *data, uint32_t bufferSize) override
    {
        for (int i = 0; i < MAX_IN_FLIGHT_FRAMES; i++)
        {
            id<MTLBuffer> buffer;
            if (data == nullptr)
            {
                buffer = [device newBufferWithLength:bufferSize
                                             options:MTLResourceCPUCacheModeWriteCombined];
            }
            else
            {
                buffer = [device newBufferWithBytes:data
                                             length:bufferSize
                                            options:MTLResourceCPUCacheModeWriteCombined];
            }

            if (buffer == nil)
                return false;

            m_buffers[i] = buffer;
        }

        m_bufferSize = bufferSize;

        return true;
    }

    void updateWithData(const void *data, uint32_t offset, uint32_t length) override
    {
        if ((length + offset) <= m_bufferSize)
        {
            uint8_t* dstPtr = (uint8_t*)[m_buffers[m_bufferIdx] contents];
            memcpy(dstPtr + offset, data, length);
        }
        else
            DF3D_ASSERT(false);
    }

    void bindBuffer(id<MTLRenderCommandEncoder> encoder, uint32_t offset) override
    {
        [encoder setVertexBuffer:m_buffers[m_bufferIdx]
                          offset:offset
                         atIndex:0];
    }

    void advanceToTheNextFrame() override
    {
        m_bufferIdx = (m_bufferIdx + 1) % MAX_IN_FLIGHT_FRAMES;
    }
    
    uint32_t getSize() const override
    {
        uint32_t result = 0;
        for (int i = 0; i < MAX_IN_FLIGHT_FRAMES; i++)
            result += m_buffers[i].length;
        return result;
    }
};

class MetalIndexBuffer
{
public:
    MTLIndexType m_indexType;
    id<MTLBuffer> m_buffer = nil;
    uint32_t m_bufferSize = 0;

    ~MetalIndexBuffer()
    {
        [m_buffer release];
    }

    bool init(id<MTLDevice> device, uint32_t numIndices, const void *data, bool indices16)
    {
        DF3D_ASSERT(data != nullptr && indices16);

        if (indices16)
            m_indexType = MTLIndexTypeUInt16;
        else
            m_indexType = MTLIndexTypeUInt32;

        m_bufferSize = numIndices * (indices16 ? sizeof(uint16_t) : sizeof(uint32_t));

        m_buffer = [device newBufferWithBytes:data length:m_bufferSize options:0];

        return m_buffer != nil;
    }
};

class MetalTexture
{
    int m_mipLevel0Width = 0;
    int m_mipLevel0Height = 0;
    PixelFormat m_format;

    id<MTLTexture> m_texture = nil;
    id<MTLSamplerState> m_samplerState = nil;

public:
    MetalTexture()
    {

    }

    ~MetalTexture()
    {
        [m_texture release];
    }

    bool init(RenderBackendMetal *backend, const TextureResourceData &data, uint32_t flags)
    {
        DF3D_ASSERT(data.mipLevels.size() > 0);

        int width = data.mipLevels[0].width;
        int height = data.mipLevels[0].height;

        [backend->m_textureDescriptor setTextureType: MTLTextureType2D];
        [backend->m_textureDescriptor setPixelFormat: GetTextureFormat(data.format)];
        [backend->m_textureDescriptor setWidth: width];
        [backend->m_textureDescriptor setHeight: height];

        bool generateMipMaps = false;
        int mipMapLevels = data.mipLevels.size();
        if (((flags & TEXTURE_FILTERING_MASK) == TEXTURE_FILTERING_TRILINEAR) ||
            ((flags & TEXTURE_FILTERING_MASK) == TEXTURE_FILTERING_ANISOTROPIC))
        {
            if (mipMapLevels == 1)
            {
                mipMapLevels = GetMipMapLevelCount(width, height);
                generateMipMaps = true;
            }
        }

        [backend->m_textureDescriptor setMipmapLevelCount: mipMapLevels];

        m_texture = [backend->m_mtlDevice newTextureWithDescriptor:backend->m_textureDescriptor];
        if (m_texture == nil)
            return false;

        for (size_t i = 0; i < data.mipLevels.size(); i++)
        {
            const auto &mipLevel = data.mipLevels[i];
            if (mipLevel.pixels.size() > 0)
            {
                int levelBytesPerRow = 0;

                if (data.format != PixelFormat::KTX)
                    levelBytesPerRow = GetBPPForFormat(data.format) * mipLevel.width;

                auto region = MTLRegionMake2D(0, 0, mipLevel.width, mipLevel.height);
                [m_texture replaceRegion:region
                             mipmapLevel:i
                               withBytes:mipLevel.pixels.data()
                             bytesPerRow:levelBytesPerRow];
            }
        }

        if (generateMipMaps)
        {
            id<MTLCommandBuffer> cmdBuf = [backend->m_commandQueue commandBuffer];
            id<MTLBlitCommandEncoder> commandEncoder = [cmdBuf blitCommandEncoder];
            [commandEncoder generateMipmapsForTexture:m_texture];
            [commandEncoder endEncoding];
            [cmdBuf commit];
            [cmdBuf waitUntilCompleted];
        }

        m_samplerState = backend->m_samplerStateCache->getOrCreate(flags);

        m_mipLevel0Width = width;
        m_mipLevel0Height = height;
        m_format = data.format;

        return true;
    }

    void update(int originX, int originY, int w, int h, const void *data)
    {
        // This method is only for TB!
        DF3D_ASSERT(w == m_mipLevel0Width && h == m_mipLevel0Height);
        auto region = MTLRegionMake2D(originX, originY, w, h);
        [m_texture replaceRegion:region
                     mipmapLevel:0
                       withBytes:data
                     bytesPerRow:GetBPPForFormat(m_format) * w];
    }

    void bind(id<MTLRenderCommandEncoder> encoder, int unit)
    {
        [encoder setFragmentTexture:m_texture atIndex:unit];
        [encoder setFragmentSamplerState:m_samplerState atIndex:unit];
    }
};

class MetalGPUProgram
{
public:
    enum UniformType
    {
        FLOAT,
        SAMPLER_IDX,
        VEC4,
        TYPE_UNDEFINED
    };

    struct Uniform
    {
        df3d::Id name;
        void *dataPointer = nullptr;
        UniformType type = TYPE_UNDEFINED;
        MetalTextureInputIndex textureKind;
    };

    id<MTLFunction> m_vertexShaderFunction = nil;
    id<MTLFunction> m_fragmentShaderFunction = nil;
    std::vector<Uniform> m_uniforms;

    MetalGPUProgram() = default;

    ~MetalGPUProgram()
    {
        DF3D_ASSERT(m_vertexShaderFunction != nil && m_fragmentShaderFunction != nil);

        [m_vertexShaderFunction release];
        [m_fragmentShaderFunction release];
    }

    bool init(id<MTLLibrary> library, const char *vertexFunctionName, const char *fragmentFunctionName)
    {
        DF3D_ASSERT(m_vertexShaderFunction == nil && m_fragmentShaderFunction == nil);

        m_vertexShaderFunction = [library newFunctionWithName:@(vertexFunctionName)];
        if (m_vertexShaderFunction == nil)
        {
            DF3D_ASSERT_MESS(false, "Failed to lookup main VERTEX shader function!");
            return false;
        }

        m_fragmentShaderFunction = [library newFunctionWithName:@(fragmentFunctionName)];
        if (m_fragmentShaderFunction == nil)
        {
            DF3D_ASSERT_MESS(false, "Failed to lookup main FRAGMENT shader function!");
            return false;
        }

        return m_vertexShaderFunction && m_fragmentShaderFunction;
    }
};

RenderBackendMetal::RenderBackendMetal(const EngineInitParams &params)
    : m_width(params.windowWidth),
    m_height(params.windowHeight),
    m_vertexBuffersBag(MemoryManager::allocDefault()),
    m_indexBuffersBag(MemoryManager::allocDefault()),
    m_texturesBag(MemoryManager::allocDefault()),
    m_gpuProgramsBag(MemoryManager::allocDefault())
{
    m_mtkView = (MTKView *)params.hardwareData;
    m_mtlDevice = m_mtkView.device;
    m_commandQueue = [m_mtlDevice newCommandQueue];
    m_defaultLibrary = [m_mtlDevice newDefaultLibrary];
    m_textureDescriptor = [MTLTextureDescriptor new];

    m_samplerStateCache = make_unique<SamplerStateCache>(m_mtlDevice);
    m_depthStencilStateCache = make_unique<DepthStencilStateCache>(m_mtlDevice);
    m_renderPipelinesCache = make_unique<RenderPipelinesCache>();

    m_caps.maxTextureSize = 4096;
    m_caps.maxAnisotropy = 16.0f;

    m_frameBoundarySemaphore = dispatch_semaphore_create(MAX_IN_FLIGHT_FRAMES);
}

RenderBackendMetal::~RenderBackendMetal()
{
    for (int i = 0; i < MAX_IN_FLIGHT_FRAMES; i++)
        dispatch_semaphore_wait(m_frameBoundarySemaphore, DISPATCH_TIME_FOREVER);

    DF3D_ASSERT(m_commandBuffer == nil);

    DF3D_ASSERT(m_dynamicBuffers.empty());
    DF3D_ASSERT(m_vertexBuffersBag.empty());
    DF3D_ASSERT(m_indexBuffersBag.empty());
    DF3D_ASSERT(m_texturesBag.empty());
    DF3D_ASSERT(m_gpuProgramsBag.empty());

    m_renderPipelinesCache.reset();
    m_depthStencilStateCache.reset();
    m_samplerStateCache.reset();

    [m_textureDescriptor release];
    [m_defaultLibrary release];
    [m_commandQueue release];

    dispatch_release(m_frameBoundarySemaphore);
}
    
const FrameStats& RenderBackendMetal::getLastFrameStats() const
{
#ifdef _DEBUG
    m_stats.gpuMemBytes = g_stats.totalMem;
#endif
    return m_stats;
}

void RenderBackendMetal::frameBegin()
{
    dispatch_semaphore_wait(m_frameBoundarySemaphore, DISPATCH_TIME_FOREVER);

    m_commandBuffer = [m_commandQueue commandBuffer];
    [m_commandBuffer retain];

    for (auto buffer : m_dynamicBuffers)
        buffer->advanceToTheNextFrame();

    m_pipelineState = {};
    m_stats.drawCalls = m_stats.totalLines = m_stats.totalTriangles = 0;

    if (MTLRenderPassDescriptor *rpd = m_mtkView.currentRenderPassDescriptor)
    {
        // Make sure to clear the buffers.
        rpd.colorAttachments[0].loadAction = MTLLoadActionClear;
        rpd.colorAttachments[0].clearColor = MTLClearColorMake(m_clearColor.x, m_clearColor.y, m_clearColor.z, 0.0);
        rpd.colorAttachments[0].storeAction = MTLStoreActionStore;

        rpd.depthAttachment.loadAction = MTLLoadActionClear;
        rpd.depthAttachment.clearDepth = m_clearDepth;
        rpd.depthAttachment.storeAction = MTLStoreActionStore;

        // Setup render pass.
        m_encoder = [m_commandBuffer renderCommandEncoderWithDescriptor:rpd];
        [m_encoder retain];

        [m_encoder setFrontFacingWinding:MTLWindingCounterClockwise];
        [m_encoder setCullMode: MTLCullModeNone];
    }
}

void RenderBackendMetal::frameEnd()
{
    [m_encoder endEncoding];

    // Finalize rendering here & push the command buffer to the GPU
    [m_commandBuffer presentDrawable:m_mtkView.currentDrawable];

    __block dispatch_semaphore_t blockSema = m_frameBoundarySemaphore;
    [m_commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
        dispatch_semaphore_signal(blockSema);
    }];
    [m_commandBuffer commit];

    [m_commandBuffer release];
    m_commandBuffer = nil;

    [m_encoder release];
    m_encoder = nil;
}

VertexBufferHandle RenderBackendMetal::createStaticVertexBuffer(const VertexFormat &format, uint32_t numVertices, const void *data)
{
    DF3D_ASSERT(numVertices > 0);

    unique_ptr<MetalVertexBuffer> vertexBuffer;
    uint32_t sizeInBytes = numVertices * format.getVertexSize();
    if (sizeInBytes < MAX_TRANSIENT_BUFFER_SIZE)
        vertexBuffer = make_unique<MetalTransientVertexBuffer>(format);
    else
        vertexBuffer = make_unique<MetalStaticVertexBuffer>(format);

    VertexBufferHandle vbHandle;
    if (vertexBuffer->initialize(m_mtlDevice, data, sizeInBytes))
    {
#ifdef _DEBUG
        g_stats.addAllocation(vertexBuffer->getSize());
#endif
        
        vbHandle = VertexBufferHandle(m_vertexBuffersBag.getNew());

        m_vertexBuffers[vbHandle.getIndex()] = std::move(vertexBuffer);
    }

    return vbHandle;
}

VertexBufferHandle RenderBackendMetal::createDynamicVertexBuffer(const VertexFormat &format, uint32_t numVertices, const void *data)
{
    DF3D_ASSERT(numVertices > 0);

    unique_ptr<MetalVertexBuffer> vertexBuffer;
    uint32_t sizeInBytes = numVertices * format.getVertexSize();
    MetalDynamicVertexBuffer *dynamicBuffer = nullptr;

    if (sizeInBytes < MAX_TRANSIENT_BUFFER_SIZE)
    {
        vertexBuffer = make_unique<MetalTransientVertexBuffer>(format);
    }
    else
    {
        auto tmp = make_unique<MetalDynamicVertexBuffer>(format);
        dynamicBuffer = tmp.get();
        vertexBuffer = std::move(tmp);
    }

    VertexBufferHandle vbHandle;
    if (vertexBuffer->initialize(m_mtlDevice, data, sizeInBytes))
    {
#ifdef _DEBUG
        g_stats.addAllocation(vertexBuffer->getSize());
#endif

        vbHandle = VertexBufferHandle(m_vertexBuffersBag.getNew());

        m_vertexBuffers[vbHandle.getIndex()] = std::move(vertexBuffer);

        if (dynamicBuffer)
            m_dynamicBuffers.push_back(dynamicBuffer);
    }

    return vbHandle;
}

void RenderBackendMetal::updateVertexBuffer(VertexBufferHandle handle, uint32_t vertexStart, uint32_t numVertices, const void *data)
{
    DF3D_ASSERT(m_vertexBuffersBag.isValid(handle.getID()));

    if (auto vb = m_vertexBuffers[handle.getIndex()].get())
    {
        auto vertexSize = vb->getFormat().getVertexSize();

        vb->updateWithData(data, vertexSize * vertexStart, vertexSize * numVertices);
    }
    else
        DF3D_ASSERT(false);
}

void RenderBackendMetal::destroyVertexBuffer(VertexBufferHandle vbHandle)
{
    DF3D_ASSERT(m_vertexBuffersBag.isValid(vbHandle.getID()));

    auto &vertexBuffer = m_vertexBuffers[vbHandle.getIndex()];

    auto foundDynamic = std::find(m_dynamicBuffers.begin(), m_dynamicBuffers.end(), vertexBuffer.get());
    if (foundDynamic != m_dynamicBuffers.end())
        m_dynamicBuffers.erase(foundDynamic);
    
#ifdef _DEBUG
    g_stats.removeAllocation(vertexBuffer->getSize());
#endif

    vertexBuffer.reset();

    m_vertexBuffersBag.release(vbHandle.getID());
}

void RenderBackendMetal::bindVertexBuffer(VertexBufferHandle handle, uint32_t vertexStart)
{
    DF3D_ASSERT(m_vertexBuffersBag.isValid(handle.getID()));

    m_pipelineState.currentVB = handle;
    m_pipelineState.vbVertexStart = vertexStart;
    m_pipelineState.indexedDrawCall = false;
}

IndexBufferHandle RenderBackendMetal::createIndexBuffer(uint32_t numIndices, const void *data, IndicesType indicesType)
{
    DF3D_ASSERT(numIndices > 0);
    DF3D_ASSERT(indicesType == INDICES_16_BIT);

    unique_ptr<MetalIndexBuffer> ibuffer = make_unique<MetalIndexBuffer>();
    IndexBufferHandle ibHandle;

    if (ibuffer->init(m_mtlDevice, numIndices, data, indicesType == INDICES_16_BIT))
    {
#ifdef _DEBUG
        g_stats.addAllocation(ibuffer->m_bufferSize);
#endif
        
        ibHandle = IndexBufferHandle(m_indexBuffersBag.getNew());
        m_indexBuffers[ibHandle.getIndex()] = std::move(ibuffer);
    }

    return ibHandle;
}

void RenderBackendMetal::destroyIndexBuffer(IndexBufferHandle ibHandle)
{
    DF3D_ASSERT(m_indexBuffersBag.isValid(ibHandle.getID()));
    
    auto &ibuffer = m_indexBuffers[ibHandle.getIndex()];
    
#ifdef _DEBUG
    g_stats.removeAllocation(ibuffer->m_bufferSize);
#endif

    ibuffer.reset();

    m_indexBuffersBag.release(ibHandle.getID());
}

void RenderBackendMetal::bindIndexBuffer(IndexBufferHandle ibHandle)
{
    DF3D_ASSERT(m_indexBuffersBag.isValid(ibHandle.getID()));

    m_pipelineState.currentIB = ibHandle;
    m_pipelineState.indexedDrawCall = true;
}

TextureHandle RenderBackendMetal::createTexture(const TextureResourceData &data, uint32_t flags)
{
    auto texture = make_unique<MetalTexture>();
    TextureHandle textureHandle;
    if (texture->init(this, data, flags))
    {
        textureHandle = TextureHandle(m_texturesBag.getNew());

        m_textures[textureHandle.getIndex()] = std::move(texture);

        m_stats.textures++;
    }

    return textureHandle;
}

void RenderBackendMetal::updateTexture(TextureHandle handle, int originX, int originY, int width, int height, const void *data)
{
    DF3D_ASSERT(m_texturesBag.isValid(handle.getID()));

    if (auto t = m_textures[handle.getIndex()].get())
        t->update(originX, originY, width, height, data);
}

void RenderBackendMetal::destroyTexture(TextureHandle textureHandle)
{
    DF3D_ASSERT(m_texturesBag.isValid(textureHandle.getID()));

    m_textures[textureHandle.getIndex()].reset();

    m_texturesBag.release(textureHandle.getID());

    DF3D_ASSERT(m_stats.textures > 0);
    m_stats.textures--;
}

void RenderBackendMetal::bindTexture(GPUProgramHandle program, TextureHandle handle, UniformHandle textureUniform, int unit)
{
    DF3D_ASSERT(m_texturesBag.isValid(handle.getID()));

    DF3D_ASSERT(unit >= 0 && unit < MAX_TEXTURE_UNITS);

    m_textureUnits[unit] = TextureUnit{ handle };

    setUniformValue(program, textureUniform, &unit);
}

GPUProgramHandle RenderBackendMetal::createGPUProgram(const char *vertexShaderData, const char *fragmentShaderData)
{
    auto program = make_unique<MetalGPUProgram>();
    GPUProgramHandle programHandle;

    if (program->init(m_defaultLibrary, vertexShaderData, fragmentShaderData))
    {
        programHandle = GPUProgramHandle(m_gpuProgramsBag.getNew());
        m_programs[programHandle.getIndex()] = std::move(program);
    }

    m_renderPipelinesCache->invalidate();

    return programHandle;
}

void RenderBackendMetal::destroyGPUProgram(GPUProgramHandle programHandle)
{
    DF3D_ASSERT(m_gpuProgramsBag.isValid(programHandle.getID()));

    m_programs[programHandle.getIndex()].reset();

    m_gpuProgramsBag.release(programHandle.getID());
}

void RenderBackendMetal::bindGPUProgram(GPUProgramHandle programHandle)
{
    DF3D_ASSERT(m_gpuProgramsBag.isValid(programHandle.getID()));

    m_pipelineState.currentProgram = programHandle;
}

UniformHandle RenderBackendMetal::getUniform(GPUProgramHandle programHandle, const char *name)
{
    DF3D_ASSERT(m_gpuProgramsBag.isValid(programHandle.getID()));

    if (auto program = m_programs[programHandle.getIndex()].get())
    {
        // Find first.
        df3d::Id nameID(name);
        for (size_t i = 0; i < program->m_uniforms.size(); i++)
        {
            if (program->m_uniforms[i].name == nameID)
            {
                // Found!
                return UniformHandle(i + 1);
            }
        }

        MetalGPUProgram::Uniform uniform;
        uniform.name = nameID;
        std::string sName = name;

        if (sName == "material_diffuse") {
            uniform.type = MetalGPUProgram::VEC4;
            uniform.dataPointer = &m_uniformBuffer.userUniforms.material_diffuse;
        } else if (sName == "material_specular") {
            uniform.type = MetalGPUProgram::VEC4;
            uniform.dataPointer = &m_uniformBuffer.userUniforms.material_specular;
        } else if (sName == "material_shininess") {
            uniform.type = MetalGPUProgram::VEC4;
            uniform.dataPointer = &m_uniformBuffer.userUniforms.material_shininess;
        } else if (sName == "u_speed") {
            uniform.type = MetalGPUProgram::FLOAT;
            uniform.dataPointer = &m_uniformBuffer.userUniforms.u_speed;
        } else if (sName == "u_force") {
            uniform.type = MetalGPUProgram::FLOAT;
            uniform.dataPointer = &m_uniformBuffer.userUniforms.u_force;
        } else if (sName == "u_xTextureScale") {
            uniform.type = MetalGPUProgram::FLOAT;
            uniform.dataPointer = &m_uniformBuffer.userUniforms.u_xTextureScale;
        } else if (sName == "u_time") {
            uniform.type = MetalGPUProgram::FLOAT;
            uniform.dataPointer = &m_uniformBuffer.userUniforms.u_time;
        } else if (sName == "u_rimMinValue") {
            uniform.type = MetalGPUProgram::FLOAT;
            uniform.dataPointer = &m_uniformBuffer.userUniforms.u_rimMinValue;
        } else if (sName == "diffuseMap") {
            uniform.type = MetalGPUProgram::SAMPLER_IDX;
            uniform.textureKind = TEXTURE_IDX_DIFFUSE_MAP;
            uniform.dataPointer = &m_uniformBuffer.userUniforms.samplerIdx[TEXTURE_IDX_DIFFUSE_MAP];
        } else if (sName == "normalMap") {
            uniform.type = MetalGPUProgram::SAMPLER_IDX;
            uniform.textureKind = TEXTURE_IDX_NORMAL_MAP;
            uniform.dataPointer = &m_uniformBuffer.userUniforms.samplerIdx[TEXTURE_IDX_NORMAL_MAP];
        } else if (sName == "emissiveMap") {
            uniform.type = MetalGPUProgram::SAMPLER_IDX;
            uniform.textureKind = TEXTURE_IDX_EMISSIVE_MAP;
            uniform.dataPointer = &m_uniformBuffer.userUniforms.samplerIdx[TEXTURE_IDX_EMISSIVE_MAP];
        } else if (sName == "noiseMap") {
            uniform.type = MetalGPUProgram::SAMPLER_IDX;
            uniform.textureKind = TEXTURE_IDX_NOISE_MAP;
            uniform.dataPointer = &m_uniformBuffer.userUniforms.samplerIdx[TEXTURE_IDX_NOISE_MAP];
        } else if (sName == "u_worldViewProjectionMatrix" ||
                   sName == "u_worldViewMatrix" ||
                   sName == "u_worldViewMatrix3x3" ||
                   sName == "u_viewMatrixInverse" ||
                   sName == "u_viewMatrix" ||
                   sName == "u_projectionMatrix" ||
                   sName == "u_worldMatrix" ||
                   sName == "u_worldMatrixInverse" ||
                   sName == "u_normalMatrix" ||
                   sName == "u_globalAmbient" ||
                   sName == "u_cameraPosition" ||
                   sName == "u_fogDensity" ||
                   sName == "u_fogColor" ||
                   sName == "u_pixelSize" ||
                   sName == "u_elapsedTime" ||
                   sName == "light_0.color" ||
                   sName == "light_0.position" ||
                   sName == "light_1.color" ||
                   sName == "light_1.position")
        {
            // Pass. This is shared uniform.
        } else {
            DF3D_ASSERT_MESS(false, "Unknown user uniform! FIX THIS!");

            return {};
        }

        program->m_uniforms.push_back(uniform);

        return UniformHandle(program->m_uniforms.size());
    }

    return {};
}

void RenderBackendMetal::setUniformValue(GPUProgramHandle programHandle, UniformHandle uniformHandle, const void *data)
{
    DF3D_ASSERT(m_gpuProgramsBag.isValid(programHandle.getID()));

    if (auto program = m_programs[programHandle.getIndex()].get())
    {
        auto idx = uniformHandle.getID() - 1;

        if (idx < program->m_uniforms.size())
        {
            auto &uni = program->m_uniforms[idx];

            if (uni.type == MetalGPUProgram::FLOAT)
            {
                *((float*)uni.dataPointer) = *((float*)data);
            }
            else if (uni.type == MetalGPUProgram::VEC4)
            {
                auto input = *(glm::vec4*)data;
                auto output = (simd::float4*)uni.dataPointer;
                output->x = input.x;
                output->y = input.y;
                output->z = input.z;
                output->w = input.w;
            }
            else if (uni.type == MetalGPUProgram::SAMPLER_IDX)
            {
                *((int*)uni.dataPointer) = *((int*)data);
            }
            else
                DF3D_ASSERT(false);
        }
        else
            DF3D_ASSERT(false);
    }
}

void RenderBackendMetal::setViewport(const Viewport &viewport)
{
    if (m_encoder == nil)
    {
        DF3D_ASSERT(false);
        return;
    }

    MTLViewport vp;
    vp.originX = viewport.originX;
    vp.originY = viewport.originY;
    vp.width = viewport.width;
    vp.height = viewport.height;
    vp.znear = 0.0;
    vp.zfar = 1.0;

    [m_encoder setViewport:vp];
}

void RenderBackendMetal::setScissorTest(bool enabled, const Viewport &rect)
{
    if (m_encoder == nil)
    {
        DF3D_ASSERT(false);
        return;
    }

    MTLScissorRect scissorRect;
    if (enabled)
    {
        scissorRect = (MTLScissorRect){
            static_cast<NSUInteger>(rect.originX),
            static_cast<NSUInteger>(rect.originY),
            static_cast<NSUInteger>(rect.width),
            static_cast<NSUInteger>(rect.height) };
    }
    else
    {
        scissorRect = (MTLScissorRect){
            0,
            0,
            static_cast<NSUInteger>(m_width),
            static_cast<NSUInteger>(m_height) };
    }

    [m_encoder setScissorRect:scissorRect];
}

void RenderBackendMetal::setClearData(const glm::vec3 &color, float depth)
{
    m_clearColor = color;
    m_clearDepth = depth;
}

void RenderBackendMetal::setState(uint64_t state)
{
    m_pipelineState.state = state;
}

void RenderBackendMetal::draw(Topology type, uint32_t numberOfElements)
{
    if (m_encoder == nil)
        return;

    auto programHandle = m_pipelineState.currentProgram;
    auto vbHandle = m_pipelineState.currentVB;

    DF3D_ASSERT(m_gpuProgramsBag.isValid(programHandle.getID()) &&
                m_vertexBuffersBag.isValid(vbHandle.getID()));

    [m_encoder setDepthStencilState:m_depthStencilStateCache->getOrCreate(m_pipelineState.state)];

    auto faceCullState = m_pipelineState.state & RENDER_STATE_FACE_CULL_MASK;
    if (faceCullState == RENDER_STATE_FRONT_FACE_CW)
    {
        [m_encoder setFrontFacingWinding:MTLWindingClockwise];
        [m_encoder setCullMode: MTLCullModeBack];
    }
    else if (faceCullState == RENDER_STATE_FRONT_FACE_CCW)
    {
        [m_encoder setFrontFacingWinding:MTLWindingCounterClockwise];
        [m_encoder setCullMode: MTLCullModeBack];
    }
    else
    {
        [m_encoder setFrontFacingWinding:MTLWindingCounterClockwise];
        [m_encoder setCullMode: MTLCullModeNone];
    }

    // Create pipeline.
    auto &vb = m_vertexBuffers[vbHandle.getIndex()];
    auto &program = m_programs[programHandle.getIndex()];

    id <MTLRenderPipelineState> pipeline = m_renderPipelinesCache->getOrCreate(m_mtlDevice,
                                                                               program->m_vertexShaderFunction,
                                                                               program->m_fragmentShaderFunction,
                                                                               m_mtkView.colorPixelFormat,
                                                                               m_mtkView.depthStencilPixelFormat,
                                                                               vb->getFormat(),
                                                                               m_pipelineState.state,
                                                                               programHandle);

    [m_encoder setRenderPipelineState:pipeline];

    // Bind texures.
    for (const auto &uniform : program->m_uniforms)
    {
        if (uniform.type == MetalGPUProgram::SAMPLER_IDX)
        {
            int samplerIdx = *(int*)uniform.dataPointer;
            const auto &textureUnit = m_textureUnits[samplerIdx];
            auto textureHandle = textureUnit.textureHandle;

            DF3D_ASSERT(m_texturesBag.isValid(textureHandle.getID()));

            if (auto t = m_textures[textureHandle.getIndex()].get())
                t->bind(m_encoder, uniform.textureKind);
        }
    }

    // Pass vertex data.
    vb->bindBuffer(m_encoder, vb->getFormat().getVertexSize() * m_pipelineState.vbVertexStart);

    // Pass uniforms.
    [m_encoder setFragmentBytes:&m_uniformBuffer length:sizeof(MetalGlobalUniforms) atIndex:1];
    [m_encoder setVertexBytes:&m_uniformBuffer length:sizeof(MetalGlobalUniforms) atIndex:1];

    // Draw the stuff.
    auto primType = GetPrimitiveType(type);
    if (m_pipelineState.indexedDrawCall)
    {
        DF3D_ASSERT(m_pipelineState.currentIB.isValid());
        if (auto ib = m_indexBuffers[m_pipelineState.currentIB.getIndex()].get())
        {
            [m_encoder drawIndexedPrimitives:primType
                                  indexCount:numberOfElements
                                   indexType:ib->m_indexType
                                 indexBuffer:ib->m_buffer
                           indexBufferOffset:0];
        }
    }
    else
    {
        [m_encoder drawPrimitives:primType
                      vertexStart:0
                      vertexCount:numberOfElements];
    }

    m_pipelineState.vbVertexStart = 0;
}

}
