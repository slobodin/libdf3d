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

    void bindVertexBuffer(VertexBufferDescriptor vb) override;
    void updateVertexBuffer(VertexBufferDescriptor vb, size_t verticesCount, const void *data) override;

    IndexBufferDescriptor createIndexBuffer(size_t indicesCount, const void *data, GpuBufferUsageType usage) override;
    void destroyIndexBuffer(IndexBufferDescriptor ib) override;

    void bindIndexBuffer(IndexBufferDescriptor ib) override;
    void updateIndexBuffer(IndexBufferDescriptor ib, size_t indicesCount, const void *data) override;

    TextureDescriptor createTexture2D(const PixelBuffer &pixels, const TextureCreationParams &params) override;
    TextureDescriptor createTextureCube(unique_ptr<PixelBuffer> pixels[(size_t)CubeFace::COUNT], const TextureCreationParams &params) override;
    void destroyTexture(TextureDescriptor t) override;

    void bindTexture(TextureDescriptor t, int unit) override;

    ShaderDescriptor createShader(ShaderType type, const std::string &data) override;
    void destroyShader(ShaderDescriptor shader) override;

    GpuProgramDescriptor createGpuProgram(ShaderDescriptor vertexShader, ShaderDescriptor fragmentShader) override;
    void destroyGpuProgram(GpuProgramDescriptor program) override;

    void bindGpuProgram(GpuProgramDescriptor program) override;
    void requestUniforms(GpuProgramDescriptor program, std::vector<UniformDescriptor> &outDescr, std::vector<std::string> &outNames) override;
    void setUniformValue(UniformDescriptor uniform, const void *data) override;

    void setViewport(int x, int y, int width, int height) override;

    void clearColorBuffer(const glm::vec4 &color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) override;
    void clearDepthBuffer() override;
    void clearStencilBuffer() override;
    void enableDepthTest(bool enable) override;
    void enableDepthWrite(bool enable) override;
    void enableScissorTest(bool enable) override;
    void setScissorRegion(int x, int y, int width, int height) override;

    void setBlendingMode(BlendingMode mode) override;
    void setCullFaceMode(FaceCullMode mode) override;

    void draw(RopType type, size_t numberOfElements) override;
};

}
