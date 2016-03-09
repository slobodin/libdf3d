#pragma once

#include <libdf3d/render/IRenderBackend.h>

#if defined(DF3D_WINDOWS)
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#elif defined(DF3D_ANDROID)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#elif defined(DF3D_WINDOWS_PHONE)
#include <angle/angle_gl.h>
#include <angle/EGL/egl.h>
#include <angle/EGL/eglext.h>
#elif defined(DF3D_LINUX)
#include <GL/glew.h>
#include <GL/gl.h>
#elif defined(DF3D_MACOSX)
#include <GL/glew.h>
#include <OpenGL/gl.h>
#elif defined(DF3D_IOS)
#include <UIKit/UIKit.h>
#include <OpenGLES/EAGL.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
#error "Unsupported platform"
#endif

namespace df3d {

class Texture;

class RenderBackendGL : public IRenderBackend
{
    RenderBackendCaps m_caps;

    struct VertexBufferGL
    {
        GLuint id = GL_INVALID_ENUM;
    };

    // TODO: use array
    //std::unordered_map<, VertexBufferGL> m_vertexBuffers;

    shared_ptr<Texture> m_whiteTexture;

    void createWhiteTexture();
    void loadResidentGpuPrograms();

public:
    RenderBackendGL();
    ~RenderBackendGL();

    void createEmbedResources() override;

    const RenderBackendCaps& getCaps() const override;

    void frameBegin() override;
    void frameEnd() override;

    VertexBufferDescriptor createVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data, GpuBufferUsageType usage) override;
    void destroyVertexBuffer(VertexBufferDescriptor vb) override;

    void bindVertexBuffer(VertexBufferDescriptor vb, size_t first, size_t count) override;
    void updateVertexBuffer(VertexBufferDescriptor vb, size_t verticesCount, const void *data) override;

    IndexBufferDescriptor createIndexBuffer(size_t indicesCount, const void *data, GpuBufferUsageType usage) override;
    void destroyIndexBuffer(IndexBufferDescriptor ib) override;

    void bindIndexBuffer(IndexBufferDescriptor ib, size_t first, size_t count) override;
    void updateIndexBuffer(IndexBufferDescriptor ib, size_t indicesCount, const void *data) override;

    TextureDescriptor createTexture2D(const PixelBuffer &pixels, const TextureCreationParams &params) override;
    TextureDescriptor createTextureCube(unique_ptr<PixelBuffer> pixels[(size_t)CubeFace::COUNT], const TextureCreationParams &params) override;
    void destroyTexture(TextureDescriptor t) override;

    void bindTexture(TextureDescriptor t) override;

    ShaderDescriptor createShader(ShaderType type, const std::string &data) override;
    void destroyShader(ShaderDescriptor shader) override;

    GpuProgramDescriptor createGpuProgram(ShaderDescriptor, ShaderDescriptor) override;
    void destroyGpuProgram(GpuProgramDescriptor) override;

    void setViewport(int x, int y, int width, int height) override;
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
