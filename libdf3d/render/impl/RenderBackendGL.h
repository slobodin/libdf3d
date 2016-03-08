#pragma once

#include <libdf3d/render/IRenderBackend.h>
#include "OpenGLCommon.h"

namespace df3d {

class RenderBackendGL : public IRenderBackend
{
    struct VertexBufferGL
    {
        GLuint id = GL_INVALID_ENUM;
    };

    DescriptorPool m_descrPool;

    // TODO: use array
    std::unordered_map<Descriptor, VertexBufferGL> m_vertexBuffers;

public:
    VertexBufferDescriptor createVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data, GpuBufferUsageType usage) override;
    void destroyVertexBuffer(VertexBufferDescriptor vb) override;

    void bindVertexBuffer(VertexBufferDescriptor vb, int, int) override;
    void updateVertexBuffer(VertexBufferDescriptor vb, size_t verticesCount, const void *data) override;

    IndexBufferDescriptor createIndexBuffer(size_t indicesCount, const void *data, GpuBufferUsageType usage) override;
    void destroyIndexBuffer(IndexBufferDescriptor ib) override;

    void bindIndexBuffer(IndexBufferDescriptor ib, int, int) override;
    void updateIndexBuffer(IndexBufferDescriptor ib, size_t indicesCount, const void *data) override;

    TextureDescriptor createTexture2D() override;
    TextureDescriptor createTextureCube() override;
    void destroyTexture(TextureDescriptor t) override;

    void bindTexture(TextureDescriptor t) override;

    ShaderDescriptor createShader() override;
    void destroyShader() override;

    GpuProgramDescriptor createGpuProgram(ShaderDescriptor, ShaderDescriptor) override;
    void destroyGpuProgram(GpuProgramDescriptor) override;

    void setViewport();
    void setWorldMatrix(const glm::mat4 &worldm) override;
    void setCameraMatrix(const glm::mat4 &viewm) override;
    void setProjectionMatrix(const glm::mat4 &projm) override;

    void clearColorBuffer(const glm::vec4 &color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) override;
    void clearDepthBuffer() override;
    void clearStencilBuffer() override;
    void enableDepthTest(bool enable) override;
    void enableDepthWrite(bool enable) override;
    void enableScissorTest(bool enable) override;
    void setScissorRegion(int x, int y, int width, int height) override;

    void draw() override;
};

}
