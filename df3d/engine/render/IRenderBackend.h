#pragma once

#include "RenderCommon.h"
#include "Vertex.h"

namespace df3d {

struct RenderBackendCaps
{
    int maxTextureSize = 0;
    float maxAnisotropy = 0.0f;
};

class PixelBuffer;
class TextureCreationParams;

// Inspired by https://github.com/bkaradzic/bgfx
class IRenderBackend
{
public:
    IRenderBackend() = default;
    virtual ~IRenderBackend() = default;

    virtual const RenderBackendCaps& getCaps() const = 0;
    virtual const FrameStats& getFrameStats() const = 0;

    virtual void initialize() = 0;

    virtual void frameBegin() = 0;
    virtual void frameEnd() = 0;

    virtual VertexBufferHandle createVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data, GpuBufferUsageType usage) = 0;
    virtual void destroyVertexBuffer(VertexBufferHandle vbHandle) = 0;

    virtual void bindVertexBuffer(VertexBufferHandle vbHandle) = 0;
    virtual void updateVertexBuffer(VertexBufferHandle vbHandle, size_t verticesCount, const void *data) = 0;

    virtual IndexBufferHandle createIndexBuffer(size_t indicesCount, const void *data, GpuBufferUsageType usage) = 0;
    virtual void destroyIndexBuffer(IndexBufferHandle ibHandle) = 0;

    virtual void bindIndexBuffer(IndexBufferHandle ibHandle) = 0;
    virtual void updateIndexBuffer(IndexBufferHandle ibHandle, size_t indicesCount, const void *data) = 0;

    virtual TextureHandle createTexture2D(int width, int height, PixelFormat format, const uint8_t *data, const TextureCreationParams &params) = 0;
    virtual TextureHandle createTextureCube(unique_ptr<PixelBuffer> pixels[(size_t)CubeFace::COUNT], const TextureCreationParams &params) = 0;
    virtual void updateTexture(TextureHandle textureHandle, int w, int h, const void *data) = 0;
    virtual void destroyTexture(TextureHandle textureHandle) = 0;

    virtual void bindTexture(TextureHandle textureHandle, int unit) = 0;

    virtual ShaderHandle createShader(ShaderType type, const std::string &data) = 0;

    virtual GpuProgramHandle createGpuProgram(ShaderHandle vertexShaderHandle, ShaderHandle fragmentShaderHandle) = 0;
    virtual void destroyGpuProgram(GpuProgramHandle programHandle) = 0;

    virtual void bindGpuProgram(GpuProgramHandle programHandle) = 0;
    virtual void requestUniforms(GpuProgramHandle programHandle, std::vector<UniformHandle> &outHandles, std::vector<std::string> &outNames) = 0;
    virtual void setUniformValue(UniformHandle uniformHandle, const void *data) = 0;

    virtual void setViewport(int x, int y, int width, int height) = 0;

    virtual void clearColorBuffer(const glm::vec4 &color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) = 0;
    virtual void clearDepthBuffer() = 0;
    virtual void clearStencilBuffer() = 0;
    virtual void enableDepthTest(bool enable) = 0;
    virtual void enableDepthWrite(bool enable) = 0;
    virtual void enableScissorTest(bool enable) = 0;
    virtual void setScissorRegion(int x, int y, int width, int height) = 0;

    virtual void setBlendingMode(BlendingMode mode) = 0;
    virtual void setCullFaceMode(FaceCullMode mode) = 0;

    virtual void draw(RopType type, size_t numberOfElements) = 0;

    // NOTE: do not support other backends for now. So it's static.
    static unique_ptr<IRenderBackend> create(int width, int height);

    // Some helpers as template methods.
    VertexBufferHandle createVertexBuffer(const VertexData &data, GpuBufferUsageType usage)
    {
        return createVertexBuffer(data.getFormat(), data.getVerticesCount(), data.getRawData(), usage);
    }
};

}
