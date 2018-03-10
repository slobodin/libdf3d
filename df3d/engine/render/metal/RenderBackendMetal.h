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

class MetalVertexBuffer;
class MetalIndexBuffer;
class MetalTexture;
class MetalGPUProgram;
class SamplerStateCache;
class DepthStencilStateCache;
class RenderPipelinesCache;

class RenderBackendMetal : public IRenderBackend
{
    friend class MetalTexture;
    friend class MetalGPUProgram;

    static const int MAX_SIZE = 0xFFF;      // 4k is enough for now.

    RenderBackendCaps m_caps;
    mutable FrameStats m_stats;
    int m_width = 0;
    int m_height = 0;

    // GPU resources.
    HandleBag m_vertexBuffersBag;
    HandleBag m_indexBuffersBag;
    HandleBag m_texturesBag;
    HandleBag m_gpuProgramsBag;

    unique_ptr<MetalVertexBuffer>   m_vertexBuffers[MAX_SIZE];
    unique_ptr<MetalIndexBuffer>    m_indexBuffers[MAX_SIZE];
    unique_ptr<MetalTexture>        m_textures[MAX_SIZE];
    unique_ptr<MetalGPUProgram>     m_programs[MAX_SIZE];

    std::vector<MetalVertexBuffer*> m_dynamicBuffers;

    // State.
    struct PipelineState
    {
        uint64_t state = 0;
        GPUProgramHandle currentProgram;
        VertexBufferHandle currentVB;
        IndexBufferHandle currentIB;
        uint32_t vbVertexStart = 0;
        bool indexedDrawCall = false;
    };

    PipelineState m_pipelineState;

    MetalGlobalUniforms m_uniformBuffer;

    struct TextureUnit
    {
        TextureHandle textureHandle;
    };

    enum {
        MAX_TEXTURE_UNITS = 8
    };

    TextureUnit m_textureUnits[MAX_TEXTURE_UNITS];
    glm::vec3 m_clearColor;
    float m_clearDepth = 1.0f;

    // Metal data.
    MTKView *m_mtkView = nullptr;
    id<MTLDevice> m_mtlDevice = nil;
    id<MTLCommandQueue> m_commandQueue = nil;
    id<MTLLibrary> m_defaultLibrary = nil;
    id<MTLCommandBuffer> m_commandBuffer = nil;
    id<MTLRenderCommandEncoder> m_encoder = nil;
    dispatch_semaphore_t m_frameBoundarySemaphore;

    MTLTextureDescriptor *m_textureDescriptor = nullptr;

    unique_ptr<SamplerStateCache> m_samplerStateCache;
    unique_ptr<DepthStencilStateCache> m_depthStencilStateCache;
    unique_ptr<RenderPipelinesCache> m_renderPipelinesCache;

    void initMetal(const EngineInitParams &params);

public:
    RenderBackendMetal(const EngineInitParams &params);
    ~RenderBackendMetal();

    RenderBackendCaps getCaps() override { return m_caps; }
    FrameStats getLastFrameStats() override;
    
    void frameBegin() override;
    void frameEnd() override;

    VertexBufferHandle createStaticVertexBuffer(const VertexFormat &format, uint32_t numVertices, const void *data) override;
    VertexBufferHandle createDynamicVertexBuffer(const VertexFormat &format, uint32_t numVertices, const void *data) override;
    void destroyVertexBuffer(VertexBufferHandle handle) override;
    void bindVertexBuffer(VertexBufferHandle handle, uint32_t vertexStart) override;
    void updateVertexBuffer(VertexBufferHandle handle, uint32_t vertexStart, uint32_t numVertices, const void *data) override;

    IndexBufferHandle createIndexBuffer(uint32_t numIndices, const void *data, IndicesType indicesType) override;
    void destroyIndexBuffer(IndexBufferHandle handle) override;
    void bindIndexBuffer(IndexBufferHandle handle) override;

    TextureHandle createTexture(const TextureResourceData &data, uint32_t flags) override;
    void updateTexture(TextureHandle handle, int originX, int originY, int width, int height, const void *data) override;
    void destroyTexture(TextureHandle handle) override;
    void bindTexture(GPUProgramHandle program, TextureHandle handle, UniformHandle textureUniform, int unit) override;

    GPUProgramHandle createGPUProgram(const char *vertexShaderData, const char *fragmentShaderData) override;
    void destroyGPUProgram(GPUProgramHandle handle) override;
    void bindGPUProgram(GPUProgramHandle handle) override;

    UniformHandle getUniform(GPUProgramHandle program, const char *name) override;
    void setUniformValue(GPUProgramHandle program, UniformHandle uniformHandle, const void *data) override;

    void setViewport(const Viewport &viewport) override;
    void setScissorTest(bool enabled, const Viewport &rect) override;
    void setClearData(const glm::vec3 &color, float depth) override;
    void setState(uint64_t state) override;

    void draw(Topology type, uint32_t numberOfElements) override;

    void setDestroyAndroidWorkaround() override { }
    RenderBackendID getID() const { return RenderBackendID::METAL; }

    MetalGlobalUniforms* getGlobalUniforms() { return &m_uniformBuffer; }
};

}
