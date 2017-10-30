#include <df3d/df3d.h>
#include "RenderBackendMetal.h"

#include <df3d/engine/render/gl/GLSLPreprocess.h>
#include <_ios/AAPLShaderTypes.h>

namespace df3d {

    namespace {
        const char *SHADER_MAIN_NAME = "ShaderMain";
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

bool MetalShaderWrapper::init(id<MTLDevice> device, const char *source, ShaderType shaderType)
{
    NSError *error = nil;
    id <MTLLibrary> library = [device newLibraryWithSource:@(source) options:nil error:&error];
    if (error != nil)
    {
        DFLOG_WARN("Failed to compile Metal shader:\n\n:%s\n", [[error localizedDescription] UTF8String]);
        [library release];
        return false;
    }

    m_function = [library newFunctionWithName:@(SHADER_MAIN_NAME)];
    if (m_function == nil)
    {
        DF3D_ASSERT_MESS(false, "Failed to lookup main shader function!");
        [library release];
        return false;
    }
    m_type = shaderType;
    
    [library release];

    return true;
}

void MetalShaderWrapper::destroy()
{
    [m_function release];
    m_function = nil;
}

bool MetalGpuProgramWrapper::init(id<MTLDevice> device, MetalShaderWrapper *vShader, MetalShaderWrapper *fShader)
{
    m_vertexShader = vShader;
    m_fragmentShader = fShader;
    
    return m_vertexShader && m_fragmentShader;
}

void MetalGpuProgramWrapper::destroy()
{
    m_vertexShader = nullptr;
    m_fragmentShader = nullptr;
    m_vShaderHandle = {};
    m_fShaderHandle = {};
}
    
id <MTLRenderCommandEncoder> RenderBackendMetal::RenderPassState::createEncoder(id<MTLCommandBuffer> commandBuffer, MTLRenderPassDescriptor *renderPassDescriptor)
{
    id <MTLRenderCommandEncoder> result = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    
    [result setViewport:viewport];
    [result setCullMode:cullMode];
    [result setFrontFacingWinding:winding];
    [result setScissorRect:scissorRect];
    
    MTLDepthStencilDescriptor *dsd = [MTLDepthStencilDescriptor new];
    if (depthTestEnabled)
        dsd.depthCompareFunction = MTLCompareFunctionLessEqual;
    else
        dsd.depthCompareFunction = MTLCompareFunctionNever;
    dsd.depthWriteEnabled = depthWriteEnabled;
    
    id<MTLDepthStencilState> dss = [commandBuffer.device newDepthStencilStateWithDescriptor:dsd];
    [result setDepthStencilState:dss];

    [dss release];
    [dsd release];
    
    return result;
}

void RenderBackendMetal::initMetal(const EngineInitParams &params)
{
    m_mtkView = (MTKView *)params.hardwareData;
    m_mtlDevice = m_mtkView.device;

    m_mtkView.colorPixelFormat = MTLPixelFormatBGRA8Unorm;

    // Create the command queue
    m_commandQueue = [m_mtlDevice newCommandQueue];
}

void RenderBackendMetal::destroyShader(ShaderHandle shaderHandle)
{
    DF3D_ASSERT(m_shadersBag.isValid(shaderHandle.getID()));

    auto &shader = m_shaders[shaderHandle.getIndex()];

    shader.destroy();
    shader = {};

    m_shadersBag.release(shaderHandle.getID());
}

RenderBackendMetal::RenderBackendMetal(const EngineInitParams &params)
    : m_vertexBuffersBag(MemoryManager::allocDefault()),
    m_indexBuffersBag(MemoryManager::allocDefault()),
    m_shadersBag(MemoryManager::allocDefault()),
    m_gpuProgramsBag(MemoryManager::allocDefault()),
    m_currentPassState(params.windowWidth, params.windowHeight),
    m_width(params.windowWidth),
    m_height(params.windowHeight)
{
    initMetal(params);

    std::fill(std::begin(m_vertexBuffers), std::end(m_vertexBuffers), MetalBufferWrapper());
    std::fill(std::begin(m_indexBuffers), std::end(m_indexBuffers), MetalBufferWrapper());
    std::fill(std::begin(m_shaders), std::end(m_shaders), MetalShaderWrapper());
    std::fill(std::begin(m_programs), std::end(m_programs), MetalGpuProgramWrapper());
}

RenderBackendMetal::~RenderBackendMetal()
{
    DF3D_ASSERT(m_currentCommandBuffer == nil);

    DF3D_ASSERT(m_vertexBuffersBag.empty());
    DF3D_ASSERT(m_indexBuffersBag.empty());
    DF3D_ASSERT(m_shadersBag.empty());
    DF3D_ASSERT(m_gpuProgramsBag.empty());

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
    DF3D_ASSERT(m_currentCommandBuffer == nil);
    
    m_currentPassState = RenderPassState(m_width, m_height);
    m_currentVertexBuffer = nil;
    m_currentIndexBuffer = nil;
}

void RenderBackendMetal::frameEnd()
{
    // Finalize rendering here & push the command buffer to the GPU
    [m_currentCommandBuffer presentDrawable:m_mtkView.currentDrawable];
    [m_currentCommandBuffer commit];
    [m_currentCommandBuffer release];
    m_currentCommandBuffer = nil;
    
    m_currentVertexBuffer = nil;
    m_currentIndexBuffer = nil;
}

VertexBufferHandle RenderBackendMetal::createVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data, GpuBufferUsageType usage)
{
    DF3D_ASSERT(verticesCount > 0);

    MetalBufferWrapper bufferWrapper;
    VertexBufferHandle vbHandle;

    if (bufferWrapper.init(m_mtlDevice, data, verticesCount * format.getVertexSize()))
    {
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

    const auto &vertexBuffer = m_vertexBuffers[vbHandle.getIndex()];
    m_currentVertexBuffer = vertexBuffer.m_buffer;
    
    DF3D_ASSERT(m_currentVertexBuffer != nil);
}

void RenderBackendMetal::updateVertexBuffer(VertexBufferHandle vbHandle, size_t verticesCount, const void *data)
{
    DF3D_ASSERT(m_vertexBuffersBag.isValid(vbHandle.getID()));

    const auto &vertexBuffer = m_vertexBuffers[vbHandle.getIndex()];
    
    
}

IndexBufferHandle RenderBackendMetal::createIndexBuffer(size_t indicesCount, const void *data, GpuBufferUsageType usage, IndicesType indicesType)
{
    DF3D_ASSERT(indicesCount > 0);
    DF3D_ASSERT(indicesType == INDICES_16_BIT);

    MetalBufferWrapper bufferWrapper;
    IndexBufferHandle ibHandle;

    size_t bufferSize = indicesCount * (indicesType == INDICES_16_BIT ? sizeof(uint16_t) : sizeof(uint32_t));
    if (bufferWrapper.init(m_mtlDevice, data, bufferSize))
    {
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

    const auto &indexBuffer = m_indexBuffers[ibHandle.getIndex()];
    
    m_currentIndexBuffer = indexBuffer.m_buffer;
    
    DF3D_ASSERT(m_currentIndexBuffer != nil);
}

void RenderBackendMetal::updateIndexBuffer(IndexBufferHandle ibHandle, size_t indicesCount, const void *data)
{
    DF3D_ASSERT(m_indexBuffersBag.isValid(ibHandle.getID()));

}

TextureHandle RenderBackendMetal::createTexture2D(const TextureInfo &info, uint32_t flags, const void *data)
{
    return {};
}

TextureHandle RenderBackendMetal::createCompressedTexture(const TextureResourceData &data, uint32_t flags)
{
    return {};
}

void RenderBackendMetal::updateTexture(TextureHandle textureHandle, int w, int h, const void *data)
{

}

void RenderBackendMetal::destroyTexture(TextureHandle textureHandle)
{

}

void RenderBackendMetal::bindTexture(TextureHandle textureHandle, int unit)
{

}

ShaderHandle RenderBackendMetal::createShader(ShaderType type, const char *data)
{
    if (!data)
    {
        DFLOG_WARN("Failed to create a shader: empty shader data");
        return{};
    }

    MetalShaderWrapper shader;
    ShaderHandle shaderHandle;

    if (shader.init(m_mtlDevice, data, type))
    {
        shaderHandle = ShaderHandle(m_shadersBag.getNew());
        m_shaders[shaderHandle.getIndex()] = shader;
    }

    return shaderHandle;
}

GpuProgramHandle RenderBackendMetal::createGpuProgram(ShaderHandle vertexShaderHandle, ShaderHandle fragmentShaderHandle)
{
    DF3D_ASSERT(m_shadersBag.isValid(vertexShaderHandle.getID()));
    DF3D_ASSERT(m_shadersBag.isValid(fragmentShaderHandle.getID()));

    MetalShaderWrapper *vertexShaderGL = &m_shaders[vertexShaderHandle.getIndex()];
    MetalShaderWrapper *fragmentShaderGL = &m_shaders[fragmentShaderHandle.getIndex()];
    MetalGpuProgramWrapper program;
    GpuProgramHandle programHandle;

    if (program.init(m_mtlDevice, vertexShaderGL, fragmentShaderGL))
    {
        program.m_vShaderHandle = vertexShaderHandle;
        program.m_fShaderHandle = fragmentShaderHandle;
        programHandle = GpuProgramHandle(m_gpuProgramsBag.getNew());
        m_programs[programHandle.getIndex()] = program;
    }

    return programHandle;
}

void RenderBackendMetal::destroyGpuProgram(GpuProgramHandle programHandle)
{
    DF3D_ASSERT(m_gpuProgramsBag.isValid(programHandle.getID()));

    auto &program = m_programs[programHandle.getIndex()];

    destroyShader(program.m_vShaderHandle);
    destroyShader(program.m_fShaderHandle);

    program.destroy();
    program = {};

    m_gpuProgramsBag.release(programHandle.getID());
}

FrameBufferHandle RenderBackendMetal::createFrameBuffer(TextureHandle *attachments, size_t attachmentCount)
{
    DF3D_ASSERT_MESS(false, "RenderBackendMetal::createFrameBuffer is not implemented");
    return {};
}

void RenderBackendMetal::destroyFrameBuffer(FrameBufferHandle framebufferHandle)
{
    DF3D_ASSERT_MESS(false, "RenderBackendMetal::destroyFrameBuffer is not implemented");
}

void RenderBackendMetal::bindGpuProgram(GpuProgramHandle programHandle)
{

}

void RenderBackendMetal::requestUniforms(GpuProgramHandle programHandle, std::vector<UniformHandle> &outHandles, std::vector<std::string> &outNames)
{

}

void RenderBackendMetal::setUniformValue(UniformHandle uniformHandle, const void *data)
{

}

void RenderBackendMetal::bindFrameBuffer(FrameBufferHandle frameBufferHandle)
{
    DF3D_ASSERT_MESS(false, "RenderBackendMetal::bindFrameBuffer is not implemented");
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

}

void RenderBackendMetal::setCullFaceMode(FaceCullMode mode)
{
    m_currentPassState.setCullMode(mode);
}

void RenderBackendMetal::draw(Topology type, size_t numberOfElements)
{
    if (m_currentVertexBuffer == nil)
    {
        DF3D_ASSERT(false);
        return;
    }
    if (m_currentCommandBuffer == nil)
    {
        m_currentCommandBuffer = [m_commandQueue commandBuffer];
        [m_currentCommandBuffer retain];
    }
    
    MTLRenderPassDescriptor *renderPassDescriptor = m_mtkView.currentRenderPassDescriptor;
    // If can render to the view.
    if (renderPassDescriptor != nil)
    {
        id <MTLRenderCommandEncoder> renderEncoder = m_currentPassState.createEncoder(m_currentCommandBuffer, renderPassDescriptor);
        
//        [renderEncoder setVertexBuffer:m_currentVertexBuffer offset:0 atIndex:0];
//
//        [renderEncoder setRenderPipelineState:m_pipelineState];
//
//        // Draw the 3 vertices of our triangle
//        [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle
//                          vertexStart:0
//                          vertexCount:3];
        
        [renderEncoder endEncoding];
    }
    
    
//    if(renderPassDescriptor != nil)
//    {

//
//        // We call -[MTLRenderCommandEncoder setVertexBytes:lenght:atIndex:] tp send data from our
//        //   Application ObjC code here to our Metal 'vertexShader' function
//        // This call has 3 arguments
//        //   1) A pointer to the memory we want to pass to our shader
//        //   2) The memory size of the data we want passed down
//        //   3) An integer index which corresponds to the index of the buffer attribute qualifier
//        //      of the argument in our 'vertexShader' function
//
//        // Here we're sending a pointer to our 'triangleVertices' array (and indicating its size).
//        //   The AAPLVertexInputIndexVertices enum value corresponds to the 'vertexArray' argument
//        //   in our 'vertexShader' function because its buffer attribute qualifier also uses
//        //   AAPLVertexInputIndexVertices for its index
//        [renderEncoder setVertexBytes:triangleVertices
//                               length:sizeof(triangleVertices)
//                              atIndex:AAPLVertexInputIndexVertices];
//
//        // Here we're sending a pointer to '_viewportSize' and also indicate its size so the whole
//        //   think is passed into the shader.  The AAPLVertexInputIndexViewportSize enum value
//        ///  corresponds to the 'viewportSizePointer' argument in our 'vertexShader' function
//        //   because its buffer attribute qualifier also uses AAPLVertexInputIndexViewportSize
//        //   for its index
//        [renderEncoder setVertexBytes:&Viewport
//                               length:sizeof(Viewport)
//                              atIndex:AAPLVertexInputIndexViewportSize];
//
//        // Draw the 3 vertices of our triangle
//        [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle
//                          vertexStart:0
//                          vertexCount:3];
//
//        [renderEncoder endEncoding];
}

std::unique_ptr<IRenderBackend> IRenderBackend::create(const EngineInitParams &params)
{
    return make_unique<RenderBackendMetal>(params);
}














    std::string GLSLPreprocess::preprocess(const std::string &input, const std::string &shaderPath)
    {
        return input;
    }

    std::string GLSLPreprocess::preprocess(const std::string &input)
    {
        return input;
    }

}
