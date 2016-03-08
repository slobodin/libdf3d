#pragma once

#include "IRenderBackend.h"
#include <libdf3d/render/OpenGLCommon.h>

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
    VertexBuffer2 createVertexBuffer() override;
    void destroyVertexBuffer(VertexBuffer2 vb) override;

    void bindVertexBuffer(VertexBuffer2 vb, int, int) override;
    void updateVertexBuffer() override;

    IndexBuffer2 createIndexBuffer() override;
    void destroyIndexBuffer(IndexBuffer2 ib) override;

    void bindIndexBuffer(VertexBuffer2 vb, int, int) override;
    void updateIndexBuffer() override;

    Texture2 createTexture2D() override;
    Texture2 createTextureCube() override;
    void destroyTexture(Texture2 t) override;

    Shader2 createShader() override;
    void destroyShader() override;

    GpuProgram2 createGpuProgram(Shader2, Shader2) override;
    void destroyGpuProgram(GpuProgram2) override;

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
