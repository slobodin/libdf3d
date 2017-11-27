#include <df3d/df3d.h>
#include "RenderBackendMetal.h"



sphere_boss5.mesh shader


depth pre pass?/


optimize metal

fshader is slow (explosions and stuff on fullscreen)

resource manager should check:
    - having the .metal shader for each data/glsl
    - uniforms stated in the GLSL file should be the same as in .shader in metal
    - delete GLSL data from iOS bundle

check RenderBackendGL::frameBegin()







#include <df3d/engine/render/gl/GLSLPreprocess.h>
#include <df3d/engine/render/GpuProgramSharedState.h>

namespace df3d {

    namespace {

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

#pragma mark - Transient Buffer

MetalTransientVertexBuffer::MetalTransientVertexBuffer(const VertexFormat &format)
    : IMetalVertexBuffer(format)
{

}

MetalTransientVertexBuffer::~MetalTransientVertexBuffer()
{

}

bool MetalTransientVertexBuffer::initialize(id<MTLDevice> device, const void *data, size_t verticesCount)
{
    update(data, verticesCount);
    return true;
}

void MetalTransientVertexBuffer::update(const void *data, size_t verticesCount)
{
    auto sz = verticesCount * getFormat().getVertexSize();
    DF3D_ASSERT(sz <= m_transientStorage.size());

    m_currentSize = std::min(sz, m_transientStorage.size());

    if (data != nullptr)
        memcpy(m_transientStorage.data(), data, m_currentSize);
}

void MetalTransientVertexBuffer::bind(id<MTLRenderCommandEncoder> encoder, size_t verticesOffset)
{
    auto bytesOffset = verticesOffset * getFormat().getVertexSize();
    DF3D_ASSERT(bytesOffset < m_transientStorage.size());
    [encoder setVertexBytes:m_transientStorage.data() + bytesOffset length:m_currentSize atIndex:0];
}

#pragma mark - Static Buffer

MetalStaticVertexBuffer::MetalStaticVertexBuffer(const VertexFormat &format)
    : IMetalVertexBuffer(format)
{

}

MetalStaticVertexBuffer::~MetalStaticVertexBuffer()
{
    [m_buffer release];
}

bool MetalStaticVertexBuffer::initialize(id<MTLDevice> device, const void *data, size_t verticesCount)
{
    DF3D_ASSERT(data != nullptr);
    m_buffer = [device newBufferWithBytes:data
                                   length:verticesCount * getFormat().getVertexSize()
                                  options:MTLResourceStorageModeShared];

    return m_buffer != nil;
}

void MetalStaticVertexBuffer::update(const void *data, size_t verticesCount)
{
    DF3D_ASSERT_MESS(false, "Not implemented");
}

void MetalStaticVertexBuffer::bind(id<MTLRenderCommandEncoder> encoder, size_t verticesOffset)
{
    DF3D_ASSERT_MESS(verticesOffset == 0, "Unsupported");
    [encoder setVertexBuffer:m_buffer offset:0 atIndex:0];
}

#pragma mark - Dynamic Buffer

MetalDynamicVertexBuffer::MetalDynamicVertexBuffer(const VertexFormat &format)
    : IMetalVertexBuffer(format)
{

}

MetalDynamicVertexBuffer::~MetalDynamicVertexBuffer()
{
    for (int i = 0; i < MAX_IN_FLIGHT_FRAMES; i++)
        [m_buffers[i] release];
}

bool MetalDynamicVertexBuffer::initialize(id<MTLDevice> device, const void *data, size_t verticesCount)
{
    m_sizeInBytes = verticesCount * getFormat().getVertexSize();

    for (int i = 0; i < MAX_IN_FLIGHT_FRAMES; i++)
    {
        id<MTLBuffer> buffer;
        if (data == nullptr)
        {
            buffer = [device newBufferWithLength:m_sizeInBytes
                                         options:MTLResourceCPUCacheModeWriteCombined];
        }
        else
        {
            buffer = [device newBufferWithBytes:data
                                         length:m_sizeInBytes
                                        options:MTLResourceCPUCacheModeWriteCombined];
        }

        m_buffers[i] = buffer;
    }

    return true;
}

void MetalDynamicVertexBuffer::update(const void *data, size_t verticesCount)
{
    auto newSz = verticesCount * getFormat().getVertexSize();

    if (newSz <= m_sizeInBytes)
    {
        void* dstPtr = [m_buffers[m_bufferIdx] contents];
        memcpy(dstPtr, data, newSz);
    }
    else
        DF3D_ASSERT(false);
}

void MetalDynamicVertexBuffer::bind(id<MTLRenderCommandEncoder> encoder, size_t verticesOffset)
{
    [encoder setVertexBuffer:m_buffers[m_bufferIdx]
                      offset:verticesOffset * getFormat().getVertexSize()
                     atIndex:0];
}

void MetalDynamicVertexBuffer::advanceToTheNextFrame()
{
	m_bufferIdx = (m_bufferIdx + 1) % MAX_IN_FLIGHT_FRAMES;
}

#pragma mark - Index Buffer

bool MetalIndexBufferWrapper::init(id<MTLDevice> device, const void *data, size_t size)
{
    DF3D_ASSERT(data != nullptr);

	m_buffer = [device newBufferWithBytes:data length:size options:0];
    return m_buffer != nil;
}

void MetalIndexBufferWrapper::destroy()
{
    [m_buffer release];
    m_buffer = nil;
}

MetalTextureWrapper::MetalTextureWrapper()
{

}

MetalTextureWrapper::~MetalTextureWrapper()
{
    [m_texture release];
}

bool MetalTextureWrapper::init(RenderBackendMetal *backend, const TextureResourceData &data, uint32_t flags)
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

    m_samplerState = backend->getSamplerState(flags);

    m_mipLevel0Width = width;
    m_mipLevel0Height = height;
    m_format = data.format;

    return true;
}

void MetalTextureWrapper::update(int w, int h, const void *data)
{
    // This method is only for TB!
    DF3D_ASSERT(w == m_mipLevel0Width && h == m_mipLevel0Height);
    auto region = MTLRegionMake2D(0, 0, w, h);
    [m_texture replaceRegion:region
                 mipmapLevel:0
                   withBytes:data
                 bytesPerRow:GetBPPForFormat(m_format) * w];
}

void MetalTextureWrapper::bind(id<MTLRenderCommandEncoder> encoder, int unit)
{
    [encoder setFragmentTexture:m_texture atIndex:unit];
    [encoder setFragmentSamplerState:m_samplerState atIndex:unit];
}

bool MetalGpuProgramWrapper::init(RenderBackendMetal *backend, const char *vertexFunctionName, const char *fragmentFunctionName)
{
    DF3D_ASSERT(m_vertexShaderFunction == nil && m_fragmentShaderFunction == nil);

    auto library = backend->m_defaultLibrary;

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

void MetalGpuProgramWrapper::destroy()
{
    DF3D_ASSERT(m_vertexShaderFunction != nil && m_fragmentShaderFunction != nil);

    [m_vertexShaderFunction release];
    [m_fragmentShaderFunction release];
    m_vertexShaderFunction = nil;
    m_fragmentShaderFunction = nil;
    m_customUniforms.clear();
}

void RenderBackendMetal::initMetal(const EngineInitParams &params)
{
    m_mtkView = (MTKView *)params.hardwareData;
    m_mtlDevice = m_mtkView.device;

    m_commandQueue = [m_mtlDevice newCommandQueue];
    m_defaultLibrary = [m_mtlDevice newDefaultLibrary];
    m_textureDescriptor = [MTLTextureDescriptor new];
    m_samplerDescriptor = [MTLSamplerDescriptor new];
    m_depthStencilDescriptor = [MTLDepthStencilDescriptor new];
}

RenderBackendMetal::RenderPipelinesCache::RenderPipelinesCache(RenderBackendMetal *backend)
    : m_backend(backend)
{
	m_renderPipelineDescriptor = [MTLRenderPipelineDescriptor new];
    m_vertexDescriptor = [MTLVertexDescriptor new];
}

RenderBackendMetal::RenderPipelinesCache::~RenderPipelinesCache()
{
    invalidate();

    [m_renderPipelineDescriptor release];
    [m_vertexDescriptor release];
}

uint64_t RenderBackendMetal::RenderPipelinesCache::getHash(GpuProgramHandle program, VertexFormat vf, BlendingMode blending) const
{
    uint64_t res = (uint64_t)program.getID() << 32;
    uint32_t h1 = (uint32_t)vf.getHash() << 16;
    uint32_t h2 = (uint32_t)blending;

    return res | h1 | h2;
}

id <MTLRenderPipelineState> RenderBackendMetal::RenderPipelinesCache::getOrCreate(GpuProgramHandle program, VertexFormat vf, BlendingMode blending)
{
    auto hash = getHash(program, vf, blending);
    auto found = m_cache.find(hash);
    if (found == m_cache.end())
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

        auto &pr = m_backend->m_programs[program.getIndex()];

        [m_renderPipelineDescriptor reset];

        m_renderPipelineDescriptor.vertexFunction = pr.m_vertexShaderFunction;
        m_renderPipelineDescriptor.fragmentFunction = pr.m_fragmentShaderFunction;
        m_renderPipelineDescriptor.vertexDescriptor = m_vertexDescriptor;

        auto view = m_backend->m_mtkView;
        m_renderPipelineDescriptor.colorAttachments[0].pixelFormat = view.colorPixelFormat;
        m_renderPipelineDescriptor.depthAttachmentPixelFormat = view.depthStencilPixelFormat;

        auto colorAttachment = m_renderPipelineDescriptor.colorAttachments[0];
        switch (blending)
        {
            case BlendingMode::NONE:
                colorAttachment.blendingEnabled = false;
                break;
            case BlendingMode::ADDALPHA:
                colorAttachment.blendingEnabled = true;
                colorAttachment.sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
                colorAttachment.sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
                colorAttachment.destinationRGBBlendFactor = MTLBlendFactorOne;
                colorAttachment.destinationAlphaBlendFactor = MTLBlendFactorOne;
                break;
            case BlendingMode::ALPHA:
                colorAttachment.blendingEnabled = true;
                colorAttachment.sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
                colorAttachment.sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
                colorAttachment.destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
                colorAttachment.destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
                break;
            case BlendingMode::ADD:
                colorAttachment.blendingEnabled = true;
                colorAttachment.sourceRGBBlendFactor = MTLBlendFactorOne;
                colorAttachment.sourceAlphaBlendFactor = MTLBlendFactorOne;
                colorAttachment.destinationRGBBlendFactor = MTLBlendFactorOne;
                colorAttachment.destinationAlphaBlendFactor = MTLBlendFactorOne;
                break;
            default:
                DF3D_ASSERT(false);
                break;
        }

        NSError *error = nil;
        auto pipeline = [m_backend->m_mtlDevice newRenderPipelineStateWithDescriptor:m_renderPipelineDescriptor error:&error];
        if (pipeline == nil)
        {
            DFLOG_WARN("Failed to create pipeline: %s", error.localizedDescription.UTF8String);
            return nil;
        }

        m_cache[hash] = pipeline;
        return pipeline;
    }

    return found->second;
}

void RenderBackendMetal::RenderPipelinesCache::invalidate()
{
    for (const auto &item : m_cache)
        [item.second release];
    m_cache.clear();
}

id<MTLSamplerState> RenderBackendMetal::getSamplerState(uint32_t flags)
{
    auto found = m_samplerStateCache.find(flags);
    if (found == m_samplerStateCache.end())
    {
        SetupWrapMode(m_samplerDescriptor, flags);
        SetupTextureFiltering(m_samplerDescriptor, flags);

        m_samplerDescriptor.lodMinClamp = 0;
        m_samplerDescriptor.lodMaxClamp = FLT_MAX;
        m_samplerDescriptor.normalizedCoordinates = TRUE;

        if ((flags & TEXTURE_FILTERING_MASK) == TEXTURE_FILTERING_ANISOTROPIC)
            m_samplerDescriptor.maxAnisotropy = std::min((int)m_caps.maxAnisotropy, 16);
        else
            m_samplerDescriptor.maxAnisotropy = 1;

        auto newState = [m_mtlDevice newSamplerStateWithDescriptor: m_samplerDescriptor];

        m_samplerStateCache[flags] = newState;

        return newState;
    }

    return found->second;
}

id<MTLDepthStencilState> RenderBackendMetal::getDepthStencilState(bool depthTestEnabled, bool depthWriteEnabled)
{
    int hash = ((int)depthTestEnabled << 1) | (int)depthWriteEnabled;
    DF3D_ASSERT(hash >= 0 && hash < m_depthStencilStateCache.size());

    auto item = m_depthStencilStateCache[hash];
    if (item == nil)
    {
        m_depthStencilDescriptor.depthCompareFunction = depthTestEnabled ? MTLCompareFunctionLessEqual : MTLCompareFunctionAlways;
        m_depthStencilDescriptor.depthWriteEnabled = depthWriteEnabled;

        item = [m_mtlDevice newDepthStencilStateWithDescriptor:m_depthStencilDescriptor];

        m_depthStencilStateCache[hash] = item;
    }

    return item;
}

RenderBackendMetal::RenderBackendMetal(const EngineInitParams &params)
    : m_vertexBuffersBag(MemoryManager::allocDefault()),
    m_indexBuffersBag(MemoryManager::allocDefault()),
    m_texturesBag(MemoryManager::allocDefault()),
    m_gpuProgramsBag(MemoryManager::allocDefault()),
    m_uniformBuffer(new MetalGlobalUniforms()),
    m_width(params.windowWidth),
    m_height(params.windowHeight)
{
    initMetal(params);

    for (auto &item : m_depthStencilStateCache)
        item = nil;

    m_renderPipelinesCache = make_unique<RenderPipelinesCache>(this);

    std::fill(std::begin(m_vertexBuffers), std::end(m_vertexBuffers), nullptr);
    std::fill(std::begin(m_indexBuffers), std::end(m_indexBuffers), MetalIndexBufferWrapper());
    std::fill(std::begin(m_textures), std::end(m_textures), nullptr);
    std::fill(std::begin(m_programs), std::end(m_programs), MetalGpuProgramWrapper());

    m_caps.maxTextureSize = 4096;
    m_caps.maxAnisotropy = 16.0f;

    m_frameBoundarySemaphore = dispatch_semaphore_create(MAX_IN_FLIGHT_FRAMES);

#ifdef _DEBUG
    size_t totalStorage = sizeof(RenderBackendMetal);

    DFLOG_DEBUG("METAL STORAGE %d KB", utils::sizeKB(totalStorage));
#endif
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

    [m_textureDescriptor release];
    [m_samplerDescriptor release];
    [m_depthStencilDescriptor release];

    for (const auto &kv : m_samplerStateCache)
    {
        [kv.second release];
    }

    for (auto item : m_depthStencilStateCache)
    {
        [item release];
    }

    [m_defaultLibrary release];
    [m_commandQueue release];
}

const RenderBackendCaps& RenderBackendMetal::getCaps() const
{
    return m_caps;
}

const FrameStats& RenderBackendMetal::getFrameStats() const
{
    return m_stats;
}

void RenderBackendMetal::frameBegin()
{
    dispatch_semaphore_wait(m_frameBoundarySemaphore, DISPATCH_TIME_FOREVER);

    m_commandBuffer = [m_commandQueue commandBuffer];
    [m_commandBuffer retain];

    for (auto buffer : m_dynamicBuffers)
        buffer->advanceToTheNextFrame();

    m_currentVB = {};
    m_currentIB = {};
    m_currentProgram = {};
    m_indexedDrawCall = false;

    if (MTLRenderPassDescriptor *rpd = m_mtkView.currentRenderPassDescriptor)
    {
        // Make sure to clear the buffers.
        rpd.colorAttachments[0].loadAction = MTLLoadActionClear;
        rpd.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 0.0);
        rpd.colorAttachments[0].storeAction = MTLStoreActionStore;

        rpd.depthAttachment.loadAction = MTLLoadActionClear;
        rpd.depthAttachment.clearDepth = 1.0;
        rpd.depthAttachment.storeAction = MTLStoreActionStore;

        // Setup render pass.
        m_encoder = [m_commandBuffer renderCommandEncoderWithDescriptor:rpd];
        [m_encoder retain];

        [m_encoder setFrontFacingWinding:MTLWindingCounterClockwise];
        [m_encoder setCullMode: MTLCullModeNone];
        setViewport(0, 0, m_width, m_height);
        enableScissorTest(false);
        m_blending = BlendingMode::NONE;
        m_depthTestEnabled = true;
        m_depthWriteEnabled = true;
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

VertexBufferHandle RenderBackendMetal::createVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data)
{
    DF3D_ASSERT(verticesCount > 0);

    unique_ptr<IMetalVertexBuffer> vertexBuffer;
    size_t bytesSize = verticesCount * format.getVertexSize();
    if (bytesSize < MAX_TRANSIENT_BUFFER_SIZE)
        vertexBuffer = make_unique<MetalTransientVertexBuffer>(format);
    else
        vertexBuffer = make_unique<MetalStaticVertexBuffer>(format);

    VertexBufferHandle vbHandle;
    if (vertexBuffer->initialize(m_mtlDevice, data, verticesCount))
    {
        vbHandle = VertexBufferHandle(m_vertexBuffersBag.getNew());

        m_vertexBuffers[vbHandle.getIndex()] = std::move(vertexBuffer);
    }

    return vbHandle;
}

VertexBufferHandle RenderBackendMetal::createDynamicVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data)
{
    DF3D_ASSERT(verticesCount > 0);

    unique_ptr<IMetalVertexBuffer> vertexBuffer;
    size_t bytesSize = verticesCount * format.getVertexSize();
    MetalDynamicVertexBuffer *dynamicBuffer = nullptr;
    if (bytesSize < MAX_TRANSIENT_BUFFER_SIZE)
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
    if (vertexBuffer->initialize(m_mtlDevice, data, verticesCount))
    {
        vbHandle = VertexBufferHandle(m_vertexBuffersBag.getNew());

        m_vertexBuffers[vbHandle.getIndex()] = std::move(vertexBuffer);

        if (dynamicBuffer)
            m_dynamicBuffers.push_back(dynamicBuffer);
    }

    return vbHandle;
}

void RenderBackendMetal::updateDynamicVertexBuffer(VertexBufferHandle vbHandle, size_t verticesCount, const void *data)
{
    DF3D_ASSERT(m_vertexBuffersBag.isValid(vbHandle.getID()));

    if (auto vb = m_vertexBuffers[vbHandle.getIndex()].get())
        vb->update(data, verticesCount);
}

void RenderBackendMetal::destroyVertexBuffer(VertexBufferHandle vbHandle)
{
    DF3D_ASSERT(m_vertexBuffersBag.isValid(vbHandle.getID()));

    auto &vertexBuffer = m_vertexBuffers[vbHandle.getIndex()];

    auto foundDynamic = std::find(m_dynamicBuffers.begin(), m_dynamicBuffers.end(), vertexBuffer.get());
    if (foundDynamic != m_dynamicBuffers.end())
        m_dynamicBuffers.erase(foundDynamic);

    vertexBuffer.reset();

    m_vertexBuffersBag.release(vbHandle.getID());
}

void RenderBackendMetal::bindVertexBuffer(VertexBufferHandle vbHandle, size_t vertexBufferOffset)
{
    DF3D_ASSERT(m_vertexBuffersBag.isValid(vbHandle.getID()));

    m_currentVB = vbHandle;
    m_indexedDrawCall = false;
    m_currentVBOffset = vertexBufferOffset;
}

IndexBufferHandle RenderBackendMetal::createIndexBuffer(size_t indicesCount, const void *data, IndicesType indicesType)
{
    DF3D_ASSERT(indicesCount > 0);
    DF3D_ASSERT(indicesType == INDICES_16_BIT);

    MetalIndexBufferWrapper bufferWrapper;
    IndexBufferHandle ibHandle;

    size_t bufferSize = indicesCount * (indicesType == INDICES_16_BIT ? sizeof(uint16_t) : sizeof(uint32_t));
    if (bufferWrapper.init(m_mtlDevice, data, bufferSize))
    {
        if (indicesType == INDICES_16_BIT)
            bufferWrapper.m_indexType = MTLIndexTypeUInt16;
        else
            bufferWrapper.m_indexType = MTLIndexTypeUInt32;

        ibHandle = IndexBufferHandle(m_indexBuffersBag.getNew());
        m_indexBuffers[ibHandle.getIndex()] = bufferWrapper;
    }

    return ibHandle;
}

void RenderBackendMetal::destroyIndexBuffer(IndexBufferHandle ibHandle)
{
    DF3D_ASSERT(m_indexBuffersBag.isValid(ibHandle.getID()));

    auto &indexBuffer = m_indexBuffers[ibHandle.getIndex()];

    indexBuffer.destroy();
    indexBuffer = {};

    m_indexBuffersBag.release(ibHandle.getID());
}

void RenderBackendMetal::bindIndexBuffer(IndexBufferHandle ibHandle)
{
    DF3D_ASSERT(m_indexBuffersBag.isValid(ibHandle.getID()));

    m_currentIB = ibHandle;
    m_indexedDrawCall = true;
}

TextureHandle RenderBackendMetal::createTexture(const TextureResourceData &data, uint32_t flags)
{
    auto texture = make_unique<MetalTextureWrapper>();
    TextureHandle textureHandle;
    if (texture->init(this, data, flags))
    {
        textureHandle = TextureHandle(m_texturesBag.getNew());

        m_textures[textureHandle.getIndex()] = std::move(texture);

        m_stats.textures++;
    }

    return textureHandle;
}

void RenderBackendMetal::updateTexture(TextureHandle textureHandle, int w, int h, const void *data)
{
    DF3D_ASSERT(m_texturesBag.isValid(textureHandle.getID()));

    if (auto t = m_textures[textureHandle.getIndex()].get())
        t->update(w, h, data);
}

void RenderBackendMetal::destroyTexture(TextureHandle textureHandle)
{
    DF3D_ASSERT(m_texturesBag.isValid(textureHandle.getID()));

    auto &texture = m_textures[textureHandle.getIndex()];
    texture.reset();

    m_texturesBag.release(textureHandle.getID());

    DF3D_ASSERT(m_stats.textures > 0);
    m_stats.textures--;
}

void RenderBackendMetal::bindTexture(TextureHandle textureHandle, int unitIdx)
{
    DF3D_ASSERT(m_texturesBag.isValid(textureHandle.getID()));

    DF3D_ASSERT(unitIdx >= 0 && unitIdx < MAX_TEXTURE_UNITS);

    TextureUnit textureUnit;
    textureUnit.textureHandle = textureHandle;
    m_textureUnits[unitIdx] = textureUnit;
}

GpuProgramHandle RenderBackendMetal::createGpuProgramMetal(const char *vertexFunctionName, const char *fragmentFunctionName)
{
    MetalGpuProgramWrapper program;
    GpuProgramHandle programHandle;

    if (program.init(this, vertexFunctionName, fragmentFunctionName))
    {
        programHandle = GpuProgramHandle(m_gpuProgramsBag.getNew());
        m_programs[programHandle.getIndex()] = program;
    }

    m_renderPipelinesCache->invalidate();

    return programHandle;
}

void RenderBackendMetal::destroyGpuProgram(GpuProgramHandle programHandle)
{
    DF3D_ASSERT(m_gpuProgramsBag.isValid(programHandle.getID()));

    auto &program = m_programs[programHandle.getIndex()];

    program.destroy();
    program = {};

    m_gpuProgramsBag.release(programHandle.getID());
}

void RenderBackendMetal::bindGpuProgram(GpuProgramHandle programHandle)
{
    DF3D_ASSERT(m_gpuProgramsBag.isValid(programHandle.getID()));
    DF3D_ASSERT(m_programs[programHandle.getIndex()].m_vertexShaderFunction != nil);

    m_currentProgram = programHandle;
}

void RenderBackendMetal::requestUniforms(GpuProgramHandle programHandle,
                                         std::vector<UniformHandle> &outHandles,
                                         std::vector<std::string> &outNames)
{
    DF3D_ASSERT(m_gpuProgramsBag.isValid(programHandle.getID()));

    auto &programMtl = m_programs[programHandle.getIndex()];
    programMtl.m_customUniforms.clear();

    HandleType currHandleIdx = 0;
    for (const auto &name : outNames)
    {
        MetalGpuProgramWrapper::CustomUniform uniform;

        if (name == "material_diffuse") {
            uniform.type = MetalGpuProgramWrapper::VEC4;
            uniform.dataPointer = &m_uniformBuffer->userUniforms.material_diffuse;
        } else if (name == "material_specular") {
            uniform.type = MetalGpuProgramWrapper::VEC4;
            uniform.dataPointer = &m_uniformBuffer->userUniforms.material_specular;
        } else if (name == "material_shininess") {
            uniform.type = MetalGpuProgramWrapper::VEC4;
            uniform.dataPointer = &m_uniformBuffer->userUniforms.material_shininess;
        } else if (name == "u_speed") {
            uniform.type = MetalGpuProgramWrapper::FLOAT;
            uniform.dataPointer = &m_uniformBuffer->userUniforms.u_speed;
        } else if (name == "u_force") {
            uniform.type = MetalGpuProgramWrapper::FLOAT;
            uniform.dataPointer = &m_uniformBuffer->userUniforms.u_force;
        } else if (name == "u_xTextureScale") {
            uniform.type = MetalGpuProgramWrapper::FLOAT;
            uniform.dataPointer = &m_uniformBuffer->userUniforms.u_xTextureScale;
        } else if (name == "u_time") {
            uniform.type = MetalGpuProgramWrapper::FLOAT;
            uniform.dataPointer = &m_uniformBuffer->userUniforms.u_time;
        } else if (name == "diffuseMap") {
            uniform.type = MetalGpuProgramWrapper::SAMPLER_IDX;
            uniform.textureKind = TEXTURE_IDX_DIFFUSE_MAP;
            uniform.dataPointer = &m_uniformBuffer->userUniforms.samplerIdx[TEXTURE_IDX_DIFFUSE_MAP];
        } else if (name == "normalMap") {
            uniform.type = MetalGpuProgramWrapper::SAMPLER_IDX;
            uniform.textureKind = TEXTURE_IDX_NORMAL_MAP;
            uniform.dataPointer = &m_uniformBuffer->userUniforms.samplerIdx[TEXTURE_IDX_NORMAL_MAP];
        } else if (name == "emissiveMap") {
            uniform.type = MetalGpuProgramWrapper::SAMPLER_IDX;
            uniform.textureKind = TEXTURE_IDX_EMISSIVE_MAP;
            uniform.dataPointer = &m_uniformBuffer->userUniforms.samplerIdx[TEXTURE_IDX_EMISSIVE_MAP];
        } else if (name == "noiseMap") {
            uniform.type = MetalGpuProgramWrapper::SAMPLER_IDX;
            uniform.textureKind = TEXTURE_IDX_NOISE_MAP;
            uniform.dataPointer = &m_uniformBuffer->userUniforms.samplerIdx[TEXTURE_IDX_NOISE_MAP];
        } else {
            DF3D_ASSERT_MESS(false, "Unknown user uniform! FIX THIS!");
            continue;
        }

        programMtl.m_customUniforms.push_back(uniform);
        outHandles.push_back(UniformHandle(currHandleIdx));

        ++currHandleIdx;
    }
}

void RenderBackendMetal::setUniformValue(GpuProgramHandle programHandle, UniformHandle uniformHandle, const void *data)
{
    DF3D_ASSERT(m_gpuProgramsBag.isValid(programHandle.getID()));

    check updating

    auto &program = m_programs[programHandle.getIndex()];

    auto idx = uniformHandle.getID();
    if (idx < program.m_customUniforms.size())
    {
        auto &uni = program.m_customUniforms[idx];

        if (uni.type == MetalGpuProgramWrapper::FLOAT)
        {
            *((float*)uni.dataPointer) = *((float*)data);
        }
        else if (uni.type == MetalGpuProgramWrapper::VEC4)
        {
            auto input = *(glm::vec4*)data;
            auto output = (simd::float4*)uni.dataPointer;
            output->x = input.x;
            output->y = input.y;
            output->z = input.z;
            output->w = input.w;
        }
        else if (uni.type == MetalGpuProgramWrapper::SAMPLER_IDX)
        {
            *((int*)uni.dataPointer) = *((int*)data);
        }
        else
            DF3D_ASSERT(false);
    }
    else
        DF3D_ASSERT(false);
}

void RenderBackendMetal::setViewport(int x, int y, int width, int height)
{
    if (m_encoder == nil)
    {
        DF3D_ASSERT(false);
        return;
    }

    MTLViewport viewport;
    viewport.originX = x;
    viewport.originY = y;
    viewport.width = width;
    viewport.height = height;
    viewport.znear = 0.0;
    viewport.zfar = 1.0;

    [m_encoder setViewport:viewport];
}

void RenderBackendMetal::clearColorBuffer(const glm::vec4 &color)
{

}

void RenderBackendMetal::clearDepthBuffer()
{

}

void RenderBackendMetal::clearStencilBuffer()
{

}

void RenderBackendMetal::enableDepthTest(bool enable)
{
    m_depthTestEnabled = enable;
}

void RenderBackendMetal::enableDepthWrite(bool enable)
{
    m_depthWriteEnabled = enable;
}

void RenderBackendMetal::enableScissorTest(bool enable)
{
    if (m_encoder == nil)
    {
        DF3D_ASSERT(false);
        return;
    }

    if (!enable)
    {
        auto scissorRect = (MTLScissorRect){
            0,
            0,
            static_cast<NSUInteger>(m_width),
            static_cast<NSUInteger>(m_height) };

        [m_encoder setScissorRect:scissorRect];
    }
}

void RenderBackendMetal::setScissorRegion(int x, int y, int width, int height)
{
    if (m_encoder == nil)
    {
        DF3D_ASSERT(false);
        return;
    }

    auto scissorRect = (MTLScissorRect){
        static_cast<NSUInteger>(x),
        static_cast<NSUInteger>(y),
        static_cast<NSUInteger>(width),
        static_cast<NSUInteger>(height) };

    [m_encoder setScissorRect:scissorRect];
}

void RenderBackendMetal::setBlendingMode(BlendingMode mode)
{
    m_blending = mode;
}

void RenderBackendMetal::setCullFaceMode(FaceCullMode mode)
{
    if (m_encoder == nil)
    {
        DF3D_ASSERT(false);
        return;
    }

    MTLCullMode cullMode;
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
            return;
    }

    [m_encoder setCullMode:cullMode];
}

void RenderBackendMetal::draw(Topology type, size_t numberOfElements)
{
    if (m_encoder == nil)
        return;

    DF3D_ASSERT(m_gpuProgramsBag.isValid(m_currentProgram.getID()) &&
                m_vertexBuffersBag.isValid(m_currentVB.getID()));

    [m_encoder setDepthStencilState:getDepthStencilState(m_depthTestEnabled, m_depthWriteEnabled)];

    auto &vb = m_vertexBuffers[m_currentVB.getIndex()];
    auto &program = m_programs[m_currentProgram.getIndex()];

    // Create pipeline.
    id <MTLRenderPipelineState> pipeline = m_renderPipelinesCache->getOrCreate(m_currentProgram, vb->getFormat(), m_blending);

    [m_encoder setRenderPipelineState:pipeline];

    // Bind texures.
    for (const auto &uniform : program.m_customUniforms)
    {
        if (uniform.type == MetalGpuProgramWrapper::SAMPLER_IDX)
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
    vb->bind(m_encoder, m_currentVBOffset);

    // Pass uniforms.
    [m_encoder setFragmentBytes:m_uniformBuffer.get() length:sizeof(MetalGlobalUniforms) atIndex:1];
    [m_encoder setVertexBytes:m_uniformBuffer.get() length:sizeof(MetalGlobalUniforms) atIndex:1];

    // Draw the stuff.
    auto primType = GetPrimitiveType(type);
    if (m_indexedDrawCall)
    {
        DF3D_ASSERT(m_currentIB.isValid());
        auto &ib = m_indexBuffers[m_currentIB.getIndex()];

        [m_encoder drawIndexedPrimitives:primType
                              indexCount:numberOfElements
                               indexType:ib.m_indexType
                             indexBuffer:ib.m_buffer
                       indexBufferOffset:0];
    }
    else
    {
        [m_encoder drawPrimitives:primType
                      vertexStart:0
                      vertexCount:numberOfElements];
    }

    m_currentVBOffset = 0;
}

}
