#pragma once

#include "RenderBackendCommon.h"

namespace df3d {

class IRenderBackend
{
public:
    IRenderBackend() = default;
    virtual ~IRenderBackend() = default;

    virtual VertexBuffer2 createVertexBuffer() = 0;
    virtual void destroyVertexBuffer(VertexBuffer2 vb) = 0;

    virtual void bindVertexBuffer(VertexBuffer2 vb, int, int) = 0;
    virtual void updateVertexBuffer() = 0;

    virtual IndexBuffer2 createIndexBuffer() = 0;
    virtual void destroyIndexBuffer(IndexBuffer2 ib) = 0;

    virtual void bindIndexBuffer(VertexBuffer2 vb, int, int) = 0;
    virtual void updateIndexBuffer() = 0;

    virtual Texture2 createTexture2D() = 0;
    virtual Texture2 createTextureCube() = 0;
    virtual void destroyTexture(Texture2 t) = 0;

    virtual Shader2 createShader() = 0;
    virtual void destroyShader() = 0;

    virtual GpuProgram2 createGpuProgram(Shader2, Shader2) = 0;
    virtual void destroyGpuProgram(GpuProgram2) = 0;

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
