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

struct TextureInfo;

// Inspired by https://github.com/bkaradzic/bgfx
class IRenderBackend
{
public:
    IRenderBackend() = default;
    virtual ~IRenderBackend() = default;

    virtual const RenderBackendCaps& getCaps() const = 0;
    virtual const FrameStats& getFrameStats() const = 0;

    virtual void frameBegin() = 0;
    virtual void frameEnd() = 0;

    virtual VertexBufferHandle createVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data) = 0;
    virtual VertexBufferHandle createDynamicVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data) = 0;
    virtual void destroyVertexBuffer(VertexBufferHandle vbHandle) = 0;

    virtual void bindVertexBuffer(VertexBufferHandle vbHandle) = 0;
    virtual void updateDynamicVertexBuffer(VertexBufferHandle vbHandle, size_t verticesCount, const void *data) = 0;

    virtual IndexBufferHandle createIndexBuffer(size_t indicesCount, const void *data, IndicesType indicesType) = 0;
    virtual void destroyIndexBuffer(IndexBufferHandle ibHandle) = 0;
    virtual void bindIndexBuffer(IndexBufferHandle ibHandle) = 0;

    virtual TextureHandle createTexture2D(const TextureInfo &info, uint32_t flags, const void *data) = 0;
    virtual TextureHandle createCompressedTexture(const TextureResourceData &data, uint32_t flags) = 0;
    virtual void updateTexture(TextureHandle textureHandle, int w, int h, const void *data) = 0;
    virtual void destroyTexture(TextureHandle textureHandle) = 0;

    virtual void bindTexture(TextureHandle textureHandle, int unit) = 0;

    virtual ShaderHandle createShader(ShaderType type, const char *data) = 0;

    virtual GpuProgramHandle createGpuProgramMetal(const char *vertexFunctionName, const char *fragmentFunctionName) = 0;
    virtual GpuProgramHandle createGpuProgram(ShaderHandle vertexShaderHandle, ShaderHandle fragmentShaderHandle) = 0;
    virtual void destroyGpuProgram(GpuProgramHandle programHandle) = 0;

    virtual FrameBufferHandle createFrameBuffer(TextureHandle *attachments, size_t attachmentCount) = 0;
    virtual void destroyFrameBuffer(FrameBufferHandle framebufferHandle) = 0;

    virtual void bindGpuProgram(GpuProgramHandle programHandle) = 0;
    virtual void requestUniforms(GpuProgramHandle programHandle, std::vector<UniformHandle> &outHandles, std::vector<std::string> &outNames) = 0;
    virtual void setUniformValue(GpuProgramHandle programHandle, UniformHandle uniformHandle, const void *data) = 0;

    virtual void bindFrameBuffer(FrameBufferHandle frameBufferHandle) = 0;

    virtual void setViewport(int x, int y, int width, int height) = 0;

    virtual void clearColorBuffer(const glm::vec4 &color) = 0;
    virtual void clearDepthBuffer() = 0;
    virtual void clearStencilBuffer() = 0;
    virtual void enableDepthTest(bool enable) = 0;
    virtual void enableDepthWrite(bool enable) = 0;
    virtual void enableScissorTest(bool enable) = 0;
    virtual void setScissorRegion(int x, int y, int width, int height) = 0;

    virtual void setBlendingMode(BlendingMode mode) = 0;
    virtual void setCullFaceMode(FaceCullMode mode) = 0;

    virtual void draw(Topology type, size_t numberOfElements, size_t vertexBufferOffset) = 0;

    virtual unique_ptr<IGPUProgramSharedState> createSharedState() = 0;

    virtual void setDestroyAndroidWorkaround() = 0;
    virtual RenderBackendID getID() const = 0;
};

}
