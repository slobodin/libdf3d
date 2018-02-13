#pragma once

#include "RenderCommon.h"
#include "Vertex.h"

namespace df3d {

class IGPUProgramSharedState;
struct TextureResourceData;
struct EngineInitParams;

struct RenderBackendCaps
{
    int maxTextureSize = 0;
    float maxAnisotropy = 0.0f;
};

// Inspired by https://github.com/bkaradzic/bgfx
class IRenderBackend
{
public:
    IRenderBackend() = default;
    virtual ~IRenderBackend() = default;

    virtual const RenderBackendCaps& getCaps() const = 0;
    virtual const FrameStats& getLastFrameStats() const = 0;

    virtual void frameBegin() = 0;
    virtual void frameEnd() = 0;

    virtual VertexBufferHandle createStaticVertexBuffer(const VertexFormat &format, uint32_t numVertices, const void *data) = 0;
    virtual VertexBufferHandle createDynamicVertexBuffer(const VertexFormat &format, uint32_t numVertices, const void *data) = 0;
    virtual void destroyVertexBuffer(VertexBufferHandle handle) = 0;
    virtual void bindVertexBuffer(VertexBufferHandle handle, uint32_t vertexStart) = 0;
    virtual void updateVertexBuffer(VertexBufferHandle handle, uint32_t vertexStart, uint32_t numVertices, const void *data) = 0;

    virtual IndexBufferHandle createIndexBuffer(uint32_t numIndices, const void *data, IndicesType indicesType) = 0;
    virtual void destroyIndexBuffer(IndexBufferHandle handle) = 0;
    virtual void bindIndexBuffer(IndexBufferHandle handle) = 0;

    virtual TextureHandle createTexture(const TextureResourceData &data, uint32_t flags) = 0;
    virtual void updateTexture(TextureHandle handle, int originX, int originY, int width, int height, const void *data) = 0;
    virtual void destroyTexture(TextureHandle handle) = 0;

    virtual void bindTexture(GPUProgramHandle program, TextureHandle handle, UniformHandle textureUniform, int unit) = 0;

    virtual GPUProgramHandle createGPUProgram(const char *vertexShaderData, const char *fragmentShaderData) = 0;
    virtual void destroyGPUProgram(GPUProgramHandle handle) = 0;
    virtual void bindGPUProgram(GPUProgramHandle handle) = 0;

    virtual UniformHandle getUniform(GPUProgramHandle program, const char *name) = 0;
    virtual void setUniformValue(GPUProgramHandle program, UniformHandle uniformHandle, const void *data) = 0;

    virtual void setViewport(const Viewport &viewport) = 0;
    virtual void setScissorTest(bool enabled, const Viewport &rect) = 0;

    virtual void setClearData(const glm::vec3 &color, float depth) = 0;

    virtual void setState(uint64_t state) = 0;
    virtual void draw(Topology type, uint32_t numberOfElements) = 0;

    virtual void setDestroyAndroidWorkaround() = 0;
    virtual RenderBackendID getID() const = 0;
};

}
