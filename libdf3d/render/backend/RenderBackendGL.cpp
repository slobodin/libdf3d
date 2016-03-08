#include "RenderBackendGL.h"

namespace df3d {

df3d::VertexBuffer2 RenderBackendGL::createVertexBuffer()
{
    VertexBufferGL vb;
    glGenBuffers(1, &vb.id);

    auto id = m_descrPool.alloc();

    m_vertexBuffers[id] = vb;

    return id;
}

void RenderBackendGL::destroyVertexBuffer(VertexBuffer2 vb)
{
    m_descrPool.free(vb);

    auto glVb = m_vertexBuffers[vb];
    glDeleteBuffers(0, &glVb.id);

    m_vertexBuffers.erase(vb);
}

void RenderBackendGL::bindVertexBuffer(VertexBuffer2 vb, int, int)
{
    auto glVb = m_vertexBuffers[vb];

    glBindBuffer(GL_ARRAY_BUFFER, glVb.id);
}

void RenderBackendGL::updateVertexBuffer()
{

}

df3d::IndexBuffer2 RenderBackendGL::createIndexBuffer()
{
    return{};
}

void RenderBackendGL::destroyIndexBuffer(IndexBuffer2 ib)
{

}

void RenderBackendGL::bindIndexBuffer(VertexBuffer2 vb, int, int)
{

}

void RenderBackendGL::updateIndexBuffer()
{

}

df3d::Texture2 RenderBackendGL::createTexture2D()
{
    return{};
}

df3d::Texture2 RenderBackendGL::createTextureCube()
{
    return{};
}

void RenderBackendGL::destroyTexture(Texture2 t)
{

}

df3d::Shader2 RenderBackendGL::createShader()
{
    return{};
}

void RenderBackendGL::destroyShader()
{

}

df3d::GpuProgram2 RenderBackendGL::createGpuProgram(Shader2, Shader2)
{
    return{};
}

void RenderBackendGL::destroyGpuProgram(GpuProgram2)
{

}

void RenderBackendGL::setViewport()
{

}

void RenderBackendGL::setWorldMatrix(const glm::mat4 &worldm)
{

}

void RenderBackendGL::setCameraMatrix(const glm::mat4 &viewm)
{

}

void RenderBackendGL::setProjectionMatrix(const glm::mat4 &projm)
{

}

void RenderBackendGL::clearColorBuffer(const glm::vec4 &color)
{

}

void RenderBackendGL::clearDepthBuffer()
{

}

void RenderBackendGL::clearStencilBuffer()
{

}

void RenderBackendGL::enableDepthTest(bool enable)
{

}

void RenderBackendGL::enableDepthWrite(bool enable)
{

}

void RenderBackendGL::enableScissorTest(bool enable)
{

}

void RenderBackendGL::setScissorRegion(int x, int y, int width, int height)
{

}

void RenderBackendGL::draw()
{

}

}
