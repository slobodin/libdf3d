#pragma once

// Need to compile with "-x objective-c++". This also disables PCH.
#if defined(DF3D_IOS)
#include <df3d_pch.h>
#endif

#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/engine/render/GPUMemStats.h>
#include <df3d/lib/Handles.h>
#include <df3d/lib/Utils.h>

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

class GLVertexBuffer
{
    VertexFormat m_format;
    uint32_t m_sizeInBytes = 0;
    GLuint m_glID = 0;
    bool m_dynamic = false;

public:
    bool init(const VertexFormat &format, uint32_t verticesCount, const void *data, bool dynamic);
    void destroy();

    void bindBuffer(uint32_t vertexStart);
    void updateData(uint32_t vertexStart, uint32_t numVertices, const void *data);
    uint32_t getSize() const { return m_sizeInBytes; }
};

class GLIndexBuffer
{
    GLuint m_glID = 0;
    uint32_t m_sizeInBytes = 0;
    bool m_16Bit = false;

public:
    bool init(uint32_t numIndices, const void *data, bool indices16);
    void destroy();

    void bindBuffer();
    uint32_t getSize() const { return m_sizeInBytes; }
    bool is16Bit() const { return m_16Bit; }
};

class GLTexture
{
    uint32_t m_sizeInBytes = 0;
    GLuint m_glID = 0;
    GLenum m_glType = GL_INVALID_ENUM;
    GLenum m_glPixelFormat = GL_INVALID_ENUM;
    PixelFormat m_pixelFmt = PixelFormat::INVALID;

public:
    bool init(const TextureResourceData &data, uint32_t flags, int maxSize, float maxAniso);
    void destroy();

    void updateData(int originX, int originY, int width, int height, const void *data);
    void bindTexture(int unit);
    uint32_t getSize() const { return m_sizeInBytes; }
};

struct GLProgram
{
    struct Uniform
    {
        std::string name;
        GLenum type = GL_INVALID_ENUM;
        GLint location = -1;
    };

    std::vector<Uniform> uniforms;
    GLuint glID = 0;
};

class RenderBackendGL : public IRenderBackend
{
    RenderBackendCaps m_caps;
    FrameStats m_frameStats;

    std::string m_extensionsString;
    bool m_anisotropicFilteringSupported = false;

    HandleBag m_vertexBuffersBag;
    HandleBag m_indexBuffersBag;
    HandleBag m_texturesBag;
    HandleBag m_gpuProgramsBag;

    enum {
        MAX_SIZE = 0xFFF    // 4k is enough for now.
    };

    GLVertexBuffer m_vertexBuffers[MAX_SIZE];
    GLIndexBuffer m_indexBuffers[MAX_SIZE];
    GLTexture m_textures[MAX_SIZE];
    GLProgram m_gpuPrograms[MAX_SIZE];

    struct PipelineState
    {
        uint64_t state = 0;
        GPUProgramHandle program;
        GLenum currIndicesType = GL_INVALID_ENUM;
        bool indexedDrawCall = false;
    };

    PipelineState m_pipeLineState;

    glm::vec4 m_clearColor{ 1.0f, 1.0f, 1.0f, 1.0f };
    float m_clearDepth = 1.0f;

    bool m_destroyAndroidWorkaround = false;

    VertexBufferHandle createVBHelper(const VertexFormat &format, uint32_t numVertices, const void *data, bool dynamic);
    GLuint createShader(const char *data, GLenum type);
    void destroyShader(GLuint programID, GLuint shaderID);
    void requestUniforms(GLProgram &programGL);

    void setupDepthTest(uint64_t depthState);
    void setupDepthWrite(uint64_t dwState);
    void setupFaceCulling(uint64_t faceState);
    void setupBlending(uint64_t srcFactor, uint64_t dstFactor);

public:
    RenderBackendGL(int width, int height);
    ~RenderBackendGL();

    RenderBackendCaps getCaps() override;
    FrameStats getLastFrameStats() override;

    void frameBegin() override;
    void frameEnd() override;

    VertexBufferHandle createStaticVertexBuffer(const VertexFormat &format, uint32_t numVertices, const void *data) override;
    VertexBufferHandle createDynamicVertexBuffer(const VertexFormat &format, uint32_t numVertices, const void *data) override;
    void destroyVertexBuffer(VertexBufferHandle handle) override;
    void bindVertexBuffer(VertexBufferHandle handle, uint32_t vertexStart) override;
    void updateVertexBuffer(VertexBufferHandle handle, uint32_t vertexStart, uint32_t numVertices, const void *data) override;

    IndexBufferHandle createIndexBuffer(uint32_t numIndices, const void *data, IndicesType indicesType) override;
    void destroyIndexBuffer(IndexBufferHandle handle) override;
    void bindIndexBuffer(IndexBufferHandle handle) override;

    TextureHandle createTexture(const TextureResourceData &data, uint32_t flags) override;
    void updateTexture(TextureHandle handle, int originX, int originY, int width, int height, const void *data) override;
    void destroyTexture(TextureHandle handle) override;

    void bindTexture(GPUProgramHandle program, TextureHandle handle, UniformHandle textureUniform, int unit) override;

    GPUProgramHandle createGPUProgram(const char *vertexShaderData, const char *fragmentShaderData) override;
    void destroyGPUProgram(GPUProgramHandle handle) override;
    void bindGPUProgram(GPUProgramHandle handle) override;

    UniformHandle getUniform(GPUProgramHandle program, const char *name) override;
    void setUniformValue(GPUProgramHandle program, UniformHandle uniformHandle, const void *data) override;

    void setViewport(const Viewport &viewport) override;
    void setScissorTest(bool enabled, const Viewport &rect) override;

    void setClearData(const glm::vec3 &color, float depth) override;

    void setState(uint64_t state) override;
    void draw(Topology type, uint32_t numberOfElements) override;

    void setDestroyAndroidWorkaround() override { m_destroyAndroidWorkaround = true; }
    RenderBackendID getID() const override { return RenderBackendID::GL; }
};

}
