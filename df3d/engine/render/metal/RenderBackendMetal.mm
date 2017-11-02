#include <df3d/df3d.h>
#include "RenderBackendMetal.h"

#include <df3d/engine/render/gl/GLSLPreprocess.h>

namespace df3d {

    namespace {
        const int MAX_IN_FLIGHT_FRAMES = 3;


        int GetMipMapLevelCount(int width, int height)
        {
            auto tmpFormat = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                width:width
                                                                               height:height
                                                                            mipmapped:YES];

            return tmpFormat.mipmapLevelCount;
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
                case PixelFormat::RGB:
                    // Not supported!
                    break;
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

bool MetalBufferWrapper::init(id<MTLDevice> device, const void *data, size_t size)
{
    DF3D_ASSERT(m_buffer == nil);

    if (data == nullptr)
        m_buffer = [device newBufferWithLength:size options:0];
    else
        m_buffer = [device newBufferWithBytes:data length:size options:0];

    m_sizeInBytes = size;

    if (m_buffer == nil)
    {
        DFLOG_WARN("Failed to create Metal buffer.");
    }

    return m_buffer != nil;
}

void MetalBufferWrapper::destroy()
{
    [m_buffer release];
    m_buffer = nil;
}


bool MetalTextureWrapper::init(RenderBackendMetal *backend, const TextureInfo &info, uint32_t flags, const void *data)
{
    DF3D_ASSERT(m_texture == nil && m_samplerState == nil);

    // THIS IS USED FOR NOT COMPRESSED TEXTURES.
    DF3D_ASSERT(info.numMips == 0);

    m_info = info;

    [backend->m_textureDescriptor setTextureType: MTLTextureType2D];
    [backend->m_textureDescriptor setPixelFormat: GetTextureFormat(info.format)];
    [backend->m_textureDescriptor setWidth: info.width];
    [backend->m_textureDescriptor setHeight: info.height];

    bool genMipMaps = false;
    int mipMapLevels = 1;
    if (((flags & TEXTURE_FILTERING_MASK) == TEXTURE_FILTERING_TRILINEAR) ||
        ((flags & TEXTURE_FILTERING_MASK) == TEXTURE_FILTERING_ANISOTROPIC))
    {
        mipMapLevels = GetMipMapLevelCount(info.width, info.height);
        // Generate mip maps if not provided with texture.
        if (info.numMips == 0)
            genMipMaps = true;
    }

    [backend->m_textureDescriptor setMipmapLevelCount: mipMapLevels];

    m_texture = [backend->m_mtlDevice newTextureWithDescriptor:backend->m_textureDescriptor];

    if (data != nullptr)
    {
        auto region = MTLRegionMake2D(0, 0, info.width, info.height);
        [m_texture replaceRegion:region
                     mipmapLevel:0
                       withBytes:data
                     bytesPerRow:GetBPPForFormat(info.format) * info.width];
    }

    if (genMipMaps)
    {
        id<MTLCommandBuffer> cmdBuf = [backend->m_commandQueue commandBuffer];
        id<MTLBlitCommandEncoder> commandEncoder = [cmdBuf blitCommandEncoder];
        [commandEncoder generateMipmapsForTexture:m_texture];
        [commandEncoder endEncoding];
        [cmdBuf commit];
        [cmdBuf waitUntilCompleted];
    }

    m_samplerState = backend->getSamplerState(flags);

    return true;
}

void MetalTextureWrapper::update(int w, int h, const void *data)
{
    // This method is only for TB!
    DF3D_ASSERT(w == m_info.width && h == m_info.height);
    auto region = MTLRegionMake2D(0, 0, w, h);
    [m_texture replaceRegion:region
                 mipmapLevel:0
                   withBytes:data
                 bytesPerRow:GetBPPForFormat(m_info.format) * w];
}

void MetalTextureWrapper::destroy()
{
    DF3D_ASSERT(m_texture != nil && m_samplerState != nil);

    [m_texture release];

    m_texture = nil;
    m_samplerState = nil;   // Sampler state is in the cache. Do not release.
    m_info = {};
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
        m_renderPipelineDescriptor.stencilAttachmentPixelFormat = view.depthStencilPixelFormat;

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
    m_currentPassState(params.windowWidth, params.windowHeight),
    m_uniformBuffer(new MetalGlobalUniforms()),
    m_width(params.windowWidth),
    m_height(params.windowHeight)
{
    initMetal(params);

    for (auto &item : m_depthStencilStateCache)
        item = nil;

    m_renderPipelinesCache = make_unique<RenderPipelinesCache>(this);

    std::fill(std::begin(m_vertexBuffers), std::end(m_vertexBuffers), MetalVertexBufferWrapper());
    std::fill(std::begin(m_indexBuffers), std::end(m_indexBuffers), MetalIndexBufferWrapper());
    std::fill(std::begin(m_textures), std::end(m_textures), MetalTextureWrapper());
    std::fill(std::begin(m_programs), std::end(m_programs), MetalGpuProgramWrapper());

    m_caps.maxTextureSize = 4096;
    m_caps.maxAnisotropy = 16.0f;

    m_framesSemaphore = dispatch_semaphore_create(MAX_IN_FLIGHT_FRAMES);
}

RenderBackendMetal::~RenderBackendMetal()
{
    for (int i = 0; i < MAX_IN_FLIGHT_FRAMES; i++)
        dispatch_semaphore_wait(m_framesSemaphore, DISPATCH_TIME_FOREVER);

    DF3D_ASSERT(m_commandBuffer == nil);

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
    DF3D_ASSERT(m_commandBuffer == nil);

    m_currentPassState = RenderPassState(m_width, m_height);
    m_currentVB = {};
    m_currentIB = {};
    m_currentProgram = {};
    m_indexedDrawCall = false;
}

void RenderBackendMetal::frameEnd()
{
    if (m_commandBuffer != nil)
    {
        // Finalize rendering here & push the command buffer to the GPU
        [m_commandBuffer presentDrawable:m_mtkView.currentDrawable];

        __block dispatch_semaphore_t blockSema = m_framesSemaphore;
        [m_commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
            dispatch_semaphore_signal(blockSema);
        }];
        [m_commandBuffer commit];

        [m_commandBuffer release];
    }

    m_commandBuffer = nil;
}

VertexBufferHandle RenderBackendMetal::createVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data, GpuBufferUsageType usage)
{
    DF3D_ASSERT(verticesCount > 0);

    MetalVertexBufferWrapper bufferWrapper;
    VertexBufferHandle vbHandle;

    if (bufferWrapper.init(m_mtlDevice, data, verticesCount * format.getVertexSize()))
    {
        bufferWrapper.m_vertexFormat = format;

        vbHandle = VertexBufferHandle(m_vertexBuffersBag.getNew());

        m_vertexBuffers[vbHandle.getIndex()] = bufferWrapper;
    }

    return vbHandle;
}

void RenderBackendMetal::destroyVertexBuffer(VertexBufferHandle vbHandle)
{
    DF3D_ASSERT(m_vertexBuffersBag.isValid(vbHandle.getID()));

    auto &vertexBuffer = m_vertexBuffers[vbHandle.getIndex()];

    vertexBuffer.destroy();
    vertexBuffer = {};

    m_vertexBuffersBag.release(vbHandle.getID());
}

void RenderBackendMetal::bindVertexBuffer(VertexBufferHandle vbHandle)
{
    DF3D_ASSERT(m_vertexBuffersBag.isValid(vbHandle.getID()));
    DF3D_ASSERT(m_vertexBuffers[vbHandle.getIndex()].m_buffer != nil);

    m_currentVB = vbHandle;
    m_indexedDrawCall = false;
}

IndexBufferHandle RenderBackendMetal::createIndexBuffer(size_t indicesCount, const void *data, GpuBufferUsageType usage, IndicesType indicesType)
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
    DF3D_ASSERT(m_indexBuffers[ibHandle.getIndex()].m_buffer != nil);

    m_currentIB = ibHandle;
    m_indexedDrawCall = true;
}

TextureHandle RenderBackendMetal::createTexture2D(const TextureInfo &info, uint32_t flags, const void *data)
{
    size_t maxSize = m_caps.maxTextureSize;
    if (info.width > maxSize || info.height > maxSize)
    {
        DFLOG_WARN("Failed to create a 2D texture: size is too big. Max size: %d", maxSize);
        return{};
    }

    MetalTextureWrapper texture;
    TextureHandle textureHandle;
    if (texture.init(this, info, flags, data))
    {
        textureHandle = TextureHandle(m_texturesBag.getNew());

        m_textures[textureHandle.getIndex()] = texture;

        m_stats.textures++;
    }

    return textureHandle;
}

TextureHandle RenderBackendMetal::createCompressedTexture(const TextureResourceData &data, uint32_t flags)
{
    //////
    //////
    //////
    //////
    //////
    //////
    //////
    DF3D_ASSERT_MESS(false, "Not implemented");

    return {};
}

void RenderBackendMetal::updateTexture(TextureHandle textureHandle, int w, int h, const void *data)
{
    DF3D_ASSERT(m_texturesBag.isValid(textureHandle.getID()));

    m_textures[textureHandle.getIndex()].update(w, h, data);
}

void RenderBackendMetal::destroyTexture(TextureHandle textureHandle)
{
    DF3D_ASSERT(m_texturesBag.isValid(textureHandle.getID()));

    auto &texture = m_textures[textureHandle.getIndex()];

    texture.destroy();
    texture = {};

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
    m_currentPassState.setViewport(x, y, width, height);
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
    m_currentPassState.depthTestEnabled = enable;
}

void RenderBackendMetal::enableDepthWrite(bool enable)
{
    m_currentPassState.depthWriteEnabled = enable;
}

void RenderBackendMetal::enableScissorTest(bool enable)
{
    if (!enable)
        m_currentPassState.resetScissorRect();
}

void RenderBackendMetal::setScissorRegion(int x, int y, int width, int height)
{
    m_currentPassState.setScissorRect(x, y, width, height);
}

void RenderBackendMetal::setBlendingMode(BlendingMode mode)
{
    m_currentPassState.blendingMode = mode;
}

void RenderBackendMetal::setCullFaceMode(FaceCullMode mode)
{
    m_currentPassState.setCullMode(mode);
}

void RenderBackendMetal::draw(Topology type, size_t numberOfElements)
{
    DF3D_ASSERT(m_gpuProgramsBag.isValid(m_currentProgram.getID()) &&
                m_vertexBuffersBag.isValid(m_currentVB.getID()));

    MTLRenderPassDescriptor *rpd = m_mtkView.currentRenderPassDescriptor;
    if (rpd == nil)
        return;

    // Init command buffer.
    if (m_commandBuffer == nil)
    {
        // Wait for the oldest frame.
        dispatch_semaphore_wait(m_framesSemaphore, DISPATCH_TIME_FOREVER);

        // This is the first frame.
        m_commandBuffer = [m_commandQueue commandBuffer];
        [m_commandBuffer retain];

        // Make sure to clear the buffers.
        rpd.colorAttachments[0].loadAction = MTLLoadActionClear;
        rpd.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 0.0);
        rpd.colorAttachments[0].storeAction = MTLStoreActionStore;

        rpd.depthAttachment.loadAction = MTLLoadActionClear;
        rpd.depthAttachment.clearDepth = 1.0;
        rpd.depthAttachment.storeAction = MTLStoreActionStore;
    }
    else
    {
        // We need to overwrite previous buffer content.
        rpd.colorAttachments[0].loadAction = MTLLoadActionLoad;
        rpd.colorAttachments[0].storeAction = MTLStoreActionStore;
        rpd.depthAttachment.loadAction = MTLLoadActionLoad;
        rpd.depthAttachment.storeAction = MTLStoreActionStore;
    }

    // Setup render pass.
    id <MTLRenderCommandEncoder> encoder = [m_commandBuffer renderCommandEncoderWithDescriptor:rpd];

    [encoder setViewport:m_currentPassState.viewport];
    [encoder setCullMode:m_currentPassState.cullMode];
    [encoder setFrontFacingWinding:m_currentPassState.winding];
    [encoder setScissorRect:m_currentPassState.scissorRect];
    [encoder setDepthStencilState:getDepthStencilState(m_currentPassState.depthTestEnabled, m_currentPassState.depthWriteEnabled)];

    auto &vb = m_vertexBuffers[m_currentVB.getIndex()];
    auto &program = m_programs[m_currentProgram.getIndex()];

    // Create pipeline.
    id <MTLRenderPipelineState> pipeline = m_renderPipelinesCache->getOrCreate(m_currentProgram,
                                                                               vb.m_vertexFormat,
                                                                               m_currentPassState.blendingMode);

    [encoder setRenderPipelineState:pipeline];

    // Bind texures.
    for (const auto &uniform : program.m_customUniforms)
    {
        if (uniform.type == MetalGpuProgramWrapper::SAMPLER_IDX)
        {
            int samplerIdx = *(int*)uniform.dataPointer;
            const auto &textureUnit = m_textureUnits[samplerIdx];
            auto textureHandle = textureUnit.textureHandle;

            DF3D_ASSERT(m_texturesBag.isValid(textureHandle.getID()));

            const auto &texture = m_textures[textureHandle.getIndex()];

            int mtlIdx = uniform.textureKind;
            [encoder setFragmentTexture:texture.m_texture atIndex:mtlIdx];
            [encoder setFragmentSamplerState:texture.m_samplerState atIndex:mtlIdx];
        }
    }

    // Pass vertex data.
    [encoder setVertexBuffer:vb.m_buffer offset:0 atIndex:0];

    // Pass uniforms.
    [encoder setFragmentBytes:m_uniformBuffer.get() length:sizeof(MetalGlobalUniforms) atIndex:1];
    [encoder setVertexBytes:m_uniformBuffer.get() length:sizeof(MetalGlobalUniforms) atIndex:1];

    // Draw the stuff.
    auto primType = GetPrimitiveType(type);
    if (m_indexedDrawCall)
    {
        DF3D_ASSERT(m_currentIB.isValid());
        auto &ib = m_indexBuffers[m_currentIB.getIndex()];

        [encoder drawIndexedPrimitives:primType
                            indexCount:numberOfElements
                             indexType:ib.m_indexType
                           indexBuffer:ib.m_buffer
                     indexBufferOffset:0];
    }
    else
    {
        [encoder drawPrimitives:primType
                    vertexStart:0
                    vertexCount:numberOfElements];
    }

    [encoder endEncoding];
}

std::unique_ptr<IRenderBackend> IRenderBackend::create(const EngineInitParams &params)
{
    return make_unique<RenderBackendMetal>(params);
}

}
