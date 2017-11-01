#pragma once

// Need to compile with "-x objective-c++". This also disables PCH.
#if defined(DF3D_IOS)
#include <df3d_pch.h>
#endif

#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/engine/resources/TextureResource.h>
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

#include <map>

namespace df3d {

class GpuMemoryStats
{
    std::map<TextureHandle, size_t> m_textures;
    std::map<VertexBufferHandle, size_t> m_vertexBuffers;
    std::map<IndexBufferHandle, size_t> m_indexBuffers;

    int32_t m_total = 0;

    template<typename T, typename V>
    void addHelper(T &container, V value, size_t sizeInBytes)
    {
        DF3D_ASSERT(!utils::contains_key(container, value));
        container[value] = sizeInBytes;
        m_total += sizeInBytes;
    }

    template<typename T, typename V>
    void removeHelper(T &container, V value)
    {
        auto found = container.find(value);
        DF3D_ASSERT(found != container.end());
        m_total -= found->second;
        container.erase(found);
        DF3D_ASSERT(m_total >= 0);
    }

public:
    void addTexture(TextureHandle td, size_t sizeInBytes)
    {
        addHelper(m_textures, td, sizeInBytes);
    }

    void removeTexture(TextureHandle td)
    {
        removeHelper(m_textures, td);
    }

    void addVertexBuffer(VertexBufferHandle vb, size_t sizeInBytes)
    {
        addHelper(m_vertexBuffers, vb, sizeInBytes);
    }

    void removeVertexBuffer(VertexBufferHandle vb)
    {
        removeHelper(m_vertexBuffers, vb);
    }

    void addIndexBuffer(IndexBufferHandle ib, size_t sizeInBytes)
    {
        addHelper(m_indexBuffers, ib, sizeInBytes);
    }

    void removeIndexBuffer(IndexBufferHandle ib)
    {
        removeHelper(m_indexBuffers, ib);
    }

    size_t getGpuMemBytes() const { return m_total; }
};

struct VertexBufferGL
{
    GLuint glID = 0;
    VertexFormat format;
    size_t sizeInBytes = 0;
};

struct IndexBufferGL
{
    GLuint glID = 0;
    size_t sizeInBytes = 0;
    bool indices16bit = false;
};

struct TextureGL
{
    GLuint glID = 0;
    GLenum type = GL_INVALID_ENUM;
    GLenum pixelFormat = GL_INVALID_ENUM;
    TextureInfo info;
};

struct ShaderGL
{
    // glCreateShader returns 0 if an error occurs creating the shader object.
    GLuint glID = 0;
    GLenum type = GL_INVALID_ENUM;
};

struct ProgramGL
{
    ShaderHandle vshader;
    ShaderHandle fshader;
    GLuint glID = 0;
};

struct UniformGL
{
    GLenum type = GL_INVALID_ENUM;
    GLint location = -1;
};

struct FrameBufferGL
{
    GLuint fbo;
};

class RenderBackendGL : public IRenderBackend
{
    RenderBackendCaps m_caps;
    mutable FrameStats m_stats;

    static const int MAX_SIZE = 0xFFF;      // 4k is enough for now.

    HandleBag m_vertexBuffersBag;
    HandleBag m_indexBuffersBag;
    HandleBag m_texturesBag;
    HandleBag m_shadersBag;
    HandleBag m_gpuProgramsBag;
    HandleBag m_uniformsBag;
    HandleBag m_framebuffersBag;

    VertexBufferGL m_vertexBuffers[MAX_SIZE];
    IndexBufferGL m_indexBuffers[MAX_SIZE];
    TextureGL m_textures[MAX_SIZE];
    ShaderGL m_shaders[MAX_SIZE];
    ProgramGL m_programs[MAX_SIZE];
    UniformGL m_uniforms[MAX_SIZE];
    FrameBufferGL m_frameBuffers[MAX_SIZE];

    std::map<GpuProgramHandle, std::vector<UniformHandle>> m_programUniforms;

    // Some cached state.
    GpuProgramHandle m_currentProgram;
    VertexBufferHandle m_currentVertexBuffer;
    IndexBufferHandle m_currentIndexBuffer;
    bool m_indexedDrawCall = false;
    GLenum m_currentIndexType = GL_INVALID_ENUM;

    struct DrawState
    {
        BlendingMode blendingMode = BlendingMode::NONE;
        FaceCullMode faceCullMode = FaceCullMode::NONE;
        bool depthTest = false;
        bool depthWrite = false;
        bool scissorTest = false;
        bool blendingEnabled = false;
    };

    DrawState m_drawState;
    std::string m_extensionsString;

    bool m_anisotropicFilteringSupported = false;

#ifdef _DEBUG
    GpuMemoryStats m_gpuMemStats;
#endif

    void initExtensions();

    void destroyShader(ShaderHandle shader, GLuint programId);

    bool m_destroyAndroidWorkaround = false;

public:
    RenderBackendGL(int width, int height);
    ~RenderBackendGL();

    const RenderBackendCaps& getCaps() const override;
    const FrameStats& getFrameStats() const override;

    void frameBegin() override;
    void frameEnd() override;

    VertexBufferHandle createVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data, GpuBufferUsageType usage) override;
    void destroyVertexBuffer(VertexBufferHandle vbHandle) override;

    void bindVertexBuffer(VertexBufferHandle vbHandle) override;
    // void updateVertexBuffer(VertexBufferHandle vbHandle, size_t verticesCount, const void *data) override;

    IndexBufferHandle createIndexBuffer(size_t indicesCount, const void *data, GpuBufferUsageType usage, IndicesType indicesType) override;
    void destroyIndexBuffer(IndexBufferHandle ibHandle) override;

    void bindIndexBuffer(IndexBufferHandle ibHandle) override;
    // void updateIndexBuffer(IndexBufferHandle ibHandle, size_t indicesCount, const void *data) override;

    TextureHandle createTexture2D(const TextureInfo &info, uint32_t flags, const void *data) override;
    TextureHandle createCompressedTexture(const TextureResourceData &data, uint32_t flags) override;
    void updateTexture(TextureHandle textureHandle, int w, int h, const void *data) override;
    void destroyTexture(TextureHandle textureHandle) override;

    void bindTexture(TextureHandle textureHandle, int unit) override;

    ShaderHandle createShader(ShaderType type, const char *data) override;

    GpuProgramHandle createGpuProgram(ShaderHandle vertexShaderHandle, ShaderHandle fragmentShaderHandle) override;
    void destroyGpuProgram(GpuProgramHandle programHandle) override;

    FrameBufferHandle createFrameBuffer(TextureHandle *attachments, size_t attachmentCount) override;
    void destroyFrameBuffer(FrameBufferHandle framebufferHandle) override;

    void bindGpuProgram(GpuProgramHandle programHandle) override;
    void requestUniforms(GpuProgramHandle programHandle, std::vector<UniformHandle> &outHandles, std::vector<std::string> &outNames) override;
    void setUniformValue(UniformHandle uniformHandle, const void *data) override;

    void bindFrameBuffer(FrameBufferHandle frameBufferHandle) override;

    void setViewport(int x, int y, int width, int height) override;

    void clearColorBuffer(const glm::vec4 &color) override;
    void clearDepthBuffer() override;
    void clearStencilBuffer() override;
    void enableDepthTest(bool enable) override;
    void enableDepthWrite(bool enable) override;
    void enableScissorTest(bool enable) override;
    void setScissorRegion(int x, int y, int width, int height) override;

    void setBlendingMode(BlendingMode mode) override;
    void setCullFaceMode(FaceCullMode mode) override;

    void draw(Topology type, size_t numberOfElements) override;

    void setDestroyAndroidWorkaround() override { m_destroyAndroidWorkaround = true; }
};

}
