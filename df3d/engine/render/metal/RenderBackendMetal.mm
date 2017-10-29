#include <df3d/df3d.h>
#include "RenderBackendMetal.h"

#include <df3d/engine/render/gl/GLSLPreprocess.h>
#include <_ios/AAPLShaderTypes.h>

namespace df3d {

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

}

void RenderBackendMetal::frameEnd()
{

}

VertexBufferHandle RenderBackendMetal::createVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data, GpuBufferUsageType usage)
{
    return {};
}

void RenderBackendMetal::destroyVertexBuffer(VertexBufferHandle vbHandle)
{

}

void RenderBackendMetal::bindVertexBuffer(VertexBufferHandle vbHandle)
{

}

void RenderBackendMetal::updateVertexBuffer(VertexBufferHandle vbHandle, size_t verticesCount, const void *data)
{

}

IndexBufferHandle RenderBackendMetal::createIndexBuffer(size_t indicesCount, const void *data, GpuBufferUsageType usage, IndicesType indicesType)
{
    return {};
}

void RenderBackendMetal::destroyIndexBuffer(IndexBufferHandle ibHandle)
{

}

void RenderBackendMetal::bindIndexBuffer(IndexBufferHandle ibHandle)
{

}

void RenderBackendMetal::updateIndexBuffer(IndexBufferHandle ibHandle, size_t indicesCount, const void *data)
{

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
    return {};
}

GpuProgramHandle RenderBackendMetal::createGpuProgram(ShaderHandle vertexShaderHandle, ShaderHandle fragmentShaderHandle)
{
    return {};
}

void RenderBackendMetal::destroyGpuProgram(GpuProgramHandle programHandle)
{

}

FrameBufferHandle RenderBackendMetal::createFrameBuffer(TextureHandle *attachments, size_t attachmentCount)
{
    return {};
}

void RenderBackendMetal::destroyFrameBuffer(FrameBufferHandle framebufferHandle)
{

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

}

void RenderBackendMetal::setViewport(int x, int y, int width, int height)
{

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

}

void RenderBackendMetal::enableDepthWrite(bool enable)
{

}

void RenderBackendMetal::enableScissorTest(bool enable)
{

}

void RenderBackendMetal::setScissorRegion(int x, int y, int width, int height)
{

}

void RenderBackendMetal::setBlendingMode(BlendingMode mode)
{

}

void RenderBackendMetal::setCullFaceMode(FaceCullMode mode)
{

}

void RenderBackendMetal::draw(Topology type, size_t numberOfElements)
{
    static const AAPLVertex triangleVertices[] =
    {
        // 2D Positions,    RGBA colors
        { {  250,  -250 }, { 1, 0, 0, 1 } },
        { { -250,  -250 }, { 0, 1, 0, 1 } },
        { {    0,   250 }, { 0, 0, 1, 1 } },
    };
    
    // Create a new command buffer for each renderpass to the current drawable
    id <MTLCommandBuffer> commandBuffer = [m_commandQueue commandBuffer];
    commandBuffer.label = @"MyCommand";
    
    // Obtain a renderPassDescriptor generated from the view's drawable textures
    MTLRenderPassDescriptor *renderPassDescriptor = m_mtkView.currentRenderPassDescriptor;
    
    static vector_uint2 Viewport;
    Viewport.x = m_width;
    Viewport.y = m_height;
    
    if(renderPassDescriptor != nil)
    {
        // Create a render command encoder so we can render into something
        id <MTLRenderCommandEncoder> renderEncoder =
        [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
        renderEncoder.label = @"MyRenderEncoder";
        
        // Set the region of the drawable to which we'll draw.
        [renderEncoder setViewport:(MTLViewport){0.0, 0.0, (double)Viewport.x, (double)Viewport.y, -1.0, 1.0 }];
        
        [renderEncoder setRenderPipelineState:m_pipelineState];
        
        // We call -[MTLRenderCommandEncoder setVertexBytes:lenght:atIndex:] tp send data from our
        //   Application ObjC code here to our Metal 'vertexShader' function
        // This call has 3 arguments
        //   1) A pointer to the memory we want to pass to our shader
        //   2) The memory size of the data we want passed down
        //   3) An integer index which corresponds to the index of the buffer attribute qualifier
        //      of the argument in our 'vertexShader' function
        
        // Here we're sending a pointer to our 'triangleVertices' array (and indicating its size).
        //   The AAPLVertexInputIndexVertices enum value corresponds to the 'vertexArray' argument
        //   in our 'vertexShader' function because its buffer attribute qualifier also uses
        //   AAPLVertexInputIndexVertices for its index
        [renderEncoder setVertexBytes:triangleVertices
                               length:sizeof(triangleVertices)
                              atIndex:AAPLVertexInputIndexVertices];
        
        // Here we're sending a pointer to '_viewportSize' and also indicate its size so the whole
        //   think is passed into the shader.  The AAPLVertexInputIndexViewportSize enum value
        ///  corresponds to the 'viewportSizePointer' argument in our 'vertexShader' function
        //   because its buffer attribute qualifier also uses AAPLVertexInputIndexViewportSize
        //   for its index
        [renderEncoder setVertexBytes:&Viewport
                               length:sizeof(Viewport)
                              atIndex:AAPLVertexInputIndexViewportSize];
        
        // Draw the 3 vertices of our triangle
        [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle
                          vertexStart:0
                          vertexCount:3];
        
        [renderEncoder endEncoding];
        
        // Schedule a present once the framebuffer is complete using the current drawable
        [commandBuffer presentDrawable:m_mtkView.currentDrawable];
    }
    
    // Finalize rendering here & push the command buffer to the GPU
    [commandBuffer commit];
}

RenderBackendMetal::RenderBackendMetal(const EngineInitParams &params)
    : m_width(params.windowWidth),
    m_height(params.windowHeight)
{
    m_mtkView = (MTKView *)params.hardwareData;
    m_mtlDevice = m_mtkView.device;
    
    m_mtkView.colorPixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
    
    id<MTLLibrary> defaultLibrary = [m_mtlDevice newDefaultLibrary];
    
    // Load the vertex function from the library
    id <MTLFunction> vertexFunction = [defaultLibrary newFunctionWithName:@"vertexShader"];
    
    // Load the fragment function from the library
    id <MTLFunction> fragmentFunction = [defaultLibrary newFunctionWithName:@"fragmentShader"];
    
    // Set up a descriptor for creating a pipeline state object
    MTLRenderPipelineDescriptor *pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineStateDescriptor.label = @"Simple Pipeline";
    pipelineStateDescriptor.vertexFunction = vertexFunction;
    pipelineStateDescriptor.fragmentFunction = fragmentFunction;
    pipelineStateDescriptor.colorAttachments[0].pixelFormat = m_mtkView.colorPixelFormat;
    
    NSError *error = NULL;
    m_pipelineState = [m_mtlDevice newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                                             error:&error];
    if (!m_pipelineState)
    {
        // Pipeline State creation could fail if we haven't properly set up our pipeline descriptor.
        //  If the Metal API validation is enabled, we can find out more information about what
        //  went wrong.  (Metal API validation is enabled by default when a debug build is run
        //  from Xcode)
//        NSLog(@"Failed to created pipeline state, error %@", error);
//        return nil;
    }
    
    // Create the command queue
    m_commandQueue = [m_mtlDevice newCommandQueue];
}

RenderBackendMetal::~RenderBackendMetal()
{

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
