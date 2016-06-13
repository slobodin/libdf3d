#pragma once

// Need to compile with "-x objective-c++". This also disables PCH.
#if defined(DF3D_IOS)
#include <libdf3d/df3d_pch.h>
#endif

#include <libdf3d/render/IRenderBackend.h>
#include <libdf3d/utils/DescriptorsBag.h>

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

class GpuMemoryStats
{
    std::map<TextureDescriptor, size_t> m_textures;
    std::map<VertexBufferDescriptor, size_t> m_vertexBuffers;
    std::map<IndexBufferDescriptor, size_t> m_indexBuffers;

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
    void addTexture(TextureDescriptor td, size_t sizeInBytes) { addHelper(m_textures, td, sizeInBytes); }
    void removeTexture(TextureDescriptor td) { removeHelper(m_textures, td); }

    void addVertexBuffer(VertexBufferDescriptor vb, size_t sizeInBytes) { addHelper(m_vertexBuffers, vb, sizeInBytes); }
    void removeVertexBuffer(VertexBufferDescriptor vb) { removeHelper(m_vertexBuffers, vb); }

    void addIndexBuffer(IndexBufferDescriptor ib, size_t sizeInBytes) { addHelper(m_indexBuffers, ib, sizeInBytes); }
    void removeIndexBuffer(IndexBufferDescriptor ib) { removeHelper(m_indexBuffers, ib); }

    size_t getGpuMemBytes() const { return m_total; }
};

class RenderBackendGL : public IRenderBackend
{
    struct VertexBufferGL
    {
        GLuint gl_id = 0;
        shared_ptr<VertexFormat> format;
        size_t sizeInBytes = 0;
    };

    struct IndexBufferGL
    {
        GLuint gl_id = 0;
        size_t sizeInBytes = 0;
    };

    struct TextureGL
    {
        GLuint gl_id = 0;
        GLenum type = GL_INVALID_ENUM;
        GLint pixelFormat = 0;
    };

    struct ShaderGL
    {
        GLuint gl_id = 0;
        GLenum type = GL_INVALID_ENUM;
    };

    struct ProgramGL
    {
        GLuint gl_id = 0;
    };

    struct UniformGL
    {
        GLenum type = GL_INVALID_ENUM;
        GLint location = -1;
    };

    RenderBackendCaps m_caps;
    mutable FrameStats m_stats;

    utils::DescriptorsBag<int16_t> m_vertexBuffersBag;
    utils::DescriptorsBag<int16_t> m_indexBuffersBag;
    utils::DescriptorsBag<int16_t> m_texturesBag;
    utils::DescriptorsBag<int16_t> m_shadersBag;
    utils::DescriptorsBag<int16_t> m_gpuProgramsBag;
    utils::DescriptorsBag<int16_t> m_uniformsBag;

    static const int MAX_SIZE = 0xFFF;      // 4k is enough for now.

    VertexBufferGL m_vertexBuffers[MAX_SIZE];
    IndexBufferGL m_indexBuffers[MAX_SIZE];
    TextureGL m_textures[MAX_SIZE];
    ShaderGL m_shaders[MAX_SIZE];
    ProgramGL m_programs[MAX_SIZE];
    UniformGL m_uniforms[MAX_SIZE];

    std::unordered_map<int16_t, std::vector<UniformDescriptor>> m_programUniforms;

    // Some cached state.
    GpuProgramDescriptor m_currentProgram;
    VertexBufferDescriptor m_currentVertexBuffer;
    IndexBufferDescriptor m_currentIndexBuffer;
    bool m_indexedDrawCall = false;

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

    int m_width, m_height;

#ifdef _DEBUG
    GpuMemoryStats m_gpuMemStats;
#endif

public:
    RenderBackendGL(int width, int height);
    ~RenderBackendGL();

    const RenderBackendCaps& getCaps() const override;
    const FrameStats& getFrameStats() const override;

    void initialize() override;

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

    TextureDescriptor createTexture2D(int width, int height, PixelFormat format, const uint8_t *data, const TextureCreationParams &params) override;
    TextureDescriptor createTextureCube(unique_ptr<PixelBuffer> pixels[(size_t)CubeFace::COUNT], const TextureCreationParams &params) override;
    void updateTexture(TextureDescriptor t, int w, int h, const void *data) override;
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
