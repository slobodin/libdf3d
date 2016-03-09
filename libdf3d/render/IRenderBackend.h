#pragma once

#include "RenderCommon.h"
#include "Vertex.h"

namespace df3d {

struct RenderBackendCaps
{
    float maxAnisotropy;
};

class PixelBuffer;
class TextureCreationParams;

class IRenderBackend
{
public:
    IRenderBackend() = default;
    virtual ~IRenderBackend() = default;

    virtual const RenderBackendCaps& getCaps() const = 0;

    virtual VertexBufferDescriptor createVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data, GpuBufferUsageType usage) = 0;
    virtual void destroyVertexBuffer(VertexBufferDescriptor vb) = 0;

    virtual void bindVertexBuffer(VertexBufferDescriptor vb, size_t first, size_t count) = 0;
    virtual void updateVertexBuffer(VertexBufferDescriptor vb, size_t verticesCount, const void *data) = 0;

    virtual IndexBufferDescriptor createIndexBuffer(size_t indicesCount, const void *data, GpuBufferUsageType usage) = 0;
    virtual void destroyIndexBuffer(IndexBufferDescriptor ib) = 0;

    virtual void bindIndexBuffer(IndexBufferDescriptor ib, size_t first, size_t count) = 0;
    virtual void updateIndexBuffer(IndexBufferDescriptor ib, size_t indicesCount, const void *data) = 0;

    virtual TextureDescriptor createTexture2D(const PixelBuffer &pixels, const TextureCreationParams &params) = 0;
    virtual TextureDescriptor createTextureCube(unique_ptr<PixelBuffer> pixels[(size_t)CubeFace::COUNT], const TextureCreationParams &params) = 0;
    virtual void destroyTexture(TextureDescriptor t) = 0;
    
    virtual void bindTexture(TextureDescriptor t) = 0;

    virtual ShaderDescriptor createShader() = 0;
    virtual void destroyShader() = 0;

    virtual GpuProgramDescriptor createGpuProgram(ShaderDescriptor, ShaderDescriptor) = 0;
    virtual void destroyGpuProgram(GpuProgramDescriptor) = 0;

    virtual void setViewport() = 0;
    virtual void setWorldMatrix(const glm::mat4 &worldm) = 0;
    virtual void setCameraMatrix(const glm::mat4 &viewm) = 0;
    virtual void setProjectionMatrix(const glm::mat4 &projm) = 0;

    virtual void clearColorBuffer(const glm::vec4 &color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) = 0;
    virtual void clearDepthBuffer() = 0;
    virtual void clearStencilBuffer() = 0;
    virtual void enableDepthTest(bool enable) = 0;
    virtual void enableDepthWrite(bool enable) = 0;
    virtual void enableScissorTest(bool enable) = 0;
    virtual void setScissorRegion(int x, int y, int width, int height) = 0;

    virtual void draw() = 0;
};

}
