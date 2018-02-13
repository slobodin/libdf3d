#pragma once

// Need to compile with "-x objective-c++". This also disables PCH.
#if defined(DF3D_IOS)
#include <df3d_pch.h>
#endif

#include <df3d/engine/render/IRenderBackend.h>
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

class GPUMemoryStats;
class VertexBufferGL;
struct IndexBufferGL;
struct TextureGL;

struct UniformGL
{
    GLenum type = GL_INVALID_ENUM;
    GLint location = -1;
};

struct ProgramGL
{
    GLuint glID = 0;
    std::unordered_map<std::string, UniformHandle> uniforms;
};

class RenderBackendGL : public IRenderBackend
{
    RenderBackendCaps m_caps;
    mutable FrameStats m_stats;

#ifdef _DEBUG
    unique_ptr<GPUMemoryStats> m_gpuMemStats;
#endif

    std::string m_extensionsString;
    bool m_anisotropicFilteringSupported = false;

    HandleBag m_vertexBuffersBag;
    HandleBag m_indexBuffersBag;
    HandleBag m_texturesBag;
    HandleBag m_gpuProgramsBag;
    HandleBag m_uniformsBag;

    enum {
        MAX_SIZE = 0xFFF    // 4k is enough for now.
    };

    unique_ptr<VertexBufferGL> m_vertexBuffers[MAX_SIZE];
    unique_ptr<IndexBufferGL> m_indexBuffers[MAX_SIZE];
    unique_ptr<TextureGL> m_textures[MAX_SIZE];
    ProgramGL m_gpuPrograms[MAX_SIZE];
    UniformGL m_uniforms[MAX_SIZE];

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
    void requestUniforms(ProgramGL &programGL);

    void setupDepthTest(uint64_t depthState);
    void setupDepthWrite(uint64_t dwState);
    void setupFaceCulling(uint64_t faceState);
    void setupBlending(uint64_t srcFactor, uint64_t dstFactor);

public:
    RenderBackendGL(int width, int height);
    ~RenderBackendGL();

    const RenderBackendCaps& getCaps() const override;
    const FrameStats& getLastFrameStats() const override;

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
