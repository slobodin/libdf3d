#include "RenderBackendGL.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/render/RenderCommon.h>
#include <df3d/engine/render/RenderOperation.h>
#include <df3d/engine/resources/TextureResource.h>
#include <df3d/lib/Utils.h>

namespace df3d {

namespace {

GPUMemStats g_memStats;

const char* GLErrorCodeToString(GLenum errcode)
{
#if defined(DF3D_DESKTOP)
    return (const char *)gluErrorString(errcode);
#else
    switch (errcode)
    {
    case GL_INVALID_ENUM:
        return "Invalid enum";
    case GL_INVALID_VALUE:
        return "Invalid value";
    case GL_INVALID_OPERATION:
        return "Invalid operation";
    case GL_OUT_OF_MEMORY:
        return "Out of memory";
    default:
        return "Unknown";
    }
#endif
}

int GetTextureSize(GLint glFormat, int w, int h, bool mipmaps)
{
    float result = 0.0f;
    switch (glFormat)
    {
    case GL_RGB:
        result = 3.0f * w * h;
    case GL_RGBA:
        result = 4.0f * w * h;
    default:
        break;
    }

    if (mipmaps)
        result *= 1.33333f;

    return static_cast<int>(result);
}

#ifdef _DEBUG
#define GL_CHECK(call) do { \
        call; \
        GLenum errCode = glGetError(); \
        if (errCode != GL_NO_ERROR) \
        { \
            DFLOG_WARN("OpenGL error: %s", GLErrorCodeToString(errCode)); \
            DEBUG_BREAK();  \
        } \
    } while(0)
#else
#define GL_CHECK(call) call
#endif

#ifdef _DEBUG
#define TRACE_GPU_ALLOC(tag, size) g_memStats.traceAlloc(tag, size)
#define TRACE_GPU_FREE(tag, size) g_memStats.traceFree(tag, size)
#else
#define TRACE_GPU_ALLOC(tag, size)
#define TRACE_GPU_FREE(tag, size)
#endif

void SetupGLTextureFiltering(GLenum glType, uint32_t flags)
{
    const auto filtering = flags & TEXTURE_FILTERING_MASK;

    if (filtering == TEXTURE_FILTERING_NEAREST)
    {
        GL_CHECK(glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GL_CHECK(glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    }
    else if (filtering == TEXTURE_FILTERING_BILINEAR)
    {
        GL_CHECK(glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    }
    else if (filtering == TEXTURE_FILTERING_TRILINEAR || filtering == TEXTURE_FILTERING_ANISOTROPIC)
    {
        GL_CHECK(glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    }
    else
        DFLOG_WARN("SetupGLTextureFiltering failed");
}

void SetupGLWrapMode(GLenum glType, uint32_t flags)
{
    auto getGLWrapMode = [](uint32_t flags) {
        const auto mode = flags & TEXTURE_WRAP_MODE_MASK;

        if (mode == TEXTURE_WRAP_MODE_CLAMP)
            return GL_CLAMP_TO_EDGE;
        else if (mode == TEXTURE_WRAP_MODE_REPEAT)
            return GL_REPEAT;

        DFLOG_WARN("GetGLWrapMode was set to default: GL_REPEAT");

        return GL_REPEAT;
    };

    auto wmGL = getGLWrapMode(flags);

    GL_CHECK(glTexParameteri(glType, GL_TEXTURE_WRAP_S, wmGL));
    GL_CHECK(glTexParameteri(glType, GL_TEXTURE_WRAP_T, wmGL));
#if defined(DF3D_DESKTOP)
    GL_CHECK(glTexParameteri(glType, GL_TEXTURE_WRAP_R, wmGL));
#endif
}

GLenum GetGLDrawMode(Topology topologyType)
{
    switch (topologyType)
    {
    case Topology::LINES:
        return GL_LINES;
    case Topology::TRIANGLES:
        return GL_TRIANGLES;
    case Topology::LINE_STRIP:
        return GL_LINE_STRIP;
    case Topology::TRIANGLE_STRIP:
        return GL_TRIANGLE_STRIP;
    default:
        break;
    }

    return GL_INVALID_ENUM;
}

#ifdef _DEBUG
void PrintShaderLog(GLuint shader)
{
    int infologLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);

    unique_ptr<char[]> infoLog(new char[infologLen + 1]);
    glGetShaderInfoLog(shader, infologLen, nullptr, infoLog.get());

    DFLOG_MESS("Shader info log: %s", infoLog.get());
    DEBUG_BREAK();
}

void PrintGPUProgramLog(GLuint program)
{
    int infologLen = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLen);

    unique_ptr<char[]> infoLog(new char[infologLen + 1]);
    glGetProgramInfoLog(program, infologLen, nullptr, infoLog.get());

    DFLOG_MESS("GPU program info log: %s", infoLog.get());
    DEBUG_BREAK();
}
#endif

GLenum g_depthFuncLookup[] = {
    GL_INVALID_ENUM,

    GL_NEVER,       // RENDER_STATE_DEPTH_NEVER
    GL_LESS,        // RENDER_STATE_DEPTH_LESS
    GL_EQUAL,       // RENDER_STATE_DEPTH_EQUAL
    GL_LEQUAL,      // RENDER_STATE_DEPTH_LEQUAL
    GL_GREATER,     // RENDER_STATE_DEPTH_GREATER
    GL_NOTEQUAL,    // RENDER_STATE_DEPTH_NOTEQUAL
    GL_GEQUAL,      // RENDER_STATE_DEPTH_GEQUAL
    GL_ALWAYS       // RENDER_STATE_DEPTH_ALWAYS
};

GLenum g_blendFuncLookup[] = {
    GL_INVALID_ENUM,

    GL_ONE,                     // RENDER_STATE_BLEND_ONE
    GL_SRC_ALPHA,               // RENDER_STATE_BLEND_SRC_ALPHA
    GL_ONE_MINUS_SRC_ALPHA      // RENDER_STATE_BLEND_ONE_MINUS_SRC_ALPHA
};

}

bool GLVertexBuffer::init(const VertexFormat &format, uint32_t verticesCount, const void *data, bool dynamic)
{
    DF3D_ASSERT(verticesCount > 0);

    GL_CHECK(glGenBuffers(1, &m_glID));
    if (m_glID == 0)
    {
        DFLOG_WARN("glGenBuffers failed for GLVertexBuffer");
        return false;
    }

    m_dynamic = dynamic;
    m_format = format;
    m_sizeInBytes = verticesCount * format.getVertexSize();

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_glID));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, m_sizeInBytes, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));

    return true;
}

void GLVertexBuffer::destroy()
{
    if (m_glID)
    {
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GL_CHECK(glDeleteBuffers(1, &m_glID));
    }
    m_glID = 0;
}

void GLVertexBuffer::bindBuffer(uint32_t vertexStart)
{
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_glID));

    const auto vertexSize = m_format.getVertexSize();

    for (uint16_t i = VertexFormat::POSITION; i < VertexFormat::COUNT; i++)
    {
        auto attrib = (VertexFormat::VertexAttribute)i;

        if (!m_format.hasAttribute(attrib))
            continue;

        GL_CHECK(glEnableVertexAttribArray(attrib));

        size_t offset = m_format.getOffsetTo(attrib) + vertexStart * vertexSize;
        size_t count = m_format.getCompCount(attrib);

        GL_CHECK(glVertexAttribPointer(attrib, count, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*)offset));
    }
}

void GLVertexBuffer::updateData(uint32_t vertexStart, uint32_t numVertices, const void *data)
{
    DF3D_ASSERT(m_dynamic);

    uint32_t vertexSize = m_format.getVertexSize();
    uint32_t bytesUpdating = numVertices * vertexSize;
    uint32_t offset = vertexStart * vertexSize;

    DF3D_ASSERT(bytesUpdating + offset <= m_sizeInBytes);

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_glID));
    GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, bytesUpdating, data));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

bool GLIndexBuffer::init(uint32_t numIndices, const void *data, bool indices16)
{
    GL_CHECK(glGenBuffers(1, &m_glID));
    if (m_glID == 0)
    {
        DFLOG_WARN("glGenBuffers failed for IndexBufferGL");
        return false;
    }

    m_16Bit = indices16;
    m_sizeInBytes = numIndices * (m_16Bit ? sizeof(uint16_t) : sizeof(uint32_t));

    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glID));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_sizeInBytes, data, GL_STATIC_DRAW));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    return true;
}

void GLIndexBuffer::destroy()
{
    if (m_glID)
    {
        GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        GL_CHECK(glDeleteBuffers(1, &m_glID));
    }
    m_glID = 0;
}

void GLIndexBuffer::bindBuffer()
{
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glID));
}

bool GLTexture::init(const TextureResourceData &data, uint32_t flags, int maxSize, float maxAniso)
{
    DF3D_ASSERT(data.mipLevels.size() > 0);

    int width = data.mipLevels[0].width;
    int height = data.mipLevels[0].height;

    if (width > maxSize || height > maxSize)
    {
        DFLOG_WARN("Failed to create a texture: size is too big. Max size: %d", maxSize);
        return false;
    }

    GLenum pixelDataFormat;
    GLint glInternalFormat;
    switch (data.format)
    {
    case PixelFormat::RGB:
        pixelDataFormat = GL_RGB;
        glInternalFormat = GL_RGB;
        break;
    case PixelFormat::RGBA:
        pixelDataFormat = GL_RGBA;
        glInternalFormat = GL_RGBA;
        break;
    case PixelFormat::KTX:
        pixelDataFormat = data.glBaseInternalFormat;
        glInternalFormat = data.glInternalFormat;
        break;
    default:
        DFLOG_WARN("Invalid GL texture pixel format");
        return false;
    }

    GL_CHECK(glGenTextures(1, &m_glID));
    if (m_glID == 0)
    {
        DFLOG_WARN("glGenTextures failed");
        return false;
    }

    GLint previousUnpackAlignment;
    if (data.format == PixelFormat::KTX)
    {
        // KTX files require an unpack alignment of 4
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousUnpackAlignment);
        if (previousUnpackAlignment != 4)
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_glID));

    SetupGLWrapMode(GL_TEXTURE_2D, flags);
    SetupGLTextureFiltering(GL_TEXTURE_2D, flags);

    if (data.format == PixelFormat::KTX)
    {
        for (size_t mip = 0; mip < data.mipLevels.size(); mip++)
        {
            const auto &mipLevel = data.mipLevels[mip];

            GL_CHECK(glCompressedTexImage2D(GL_TEXTURE_2D, mip, glInternalFormat,
                                            mipLevel.width, mipLevel.height, 0, mipLevel.pixels.size(),
                                            mipLevel.pixels.data()));

            m_sizeInBytes += mipLevel.pixels.size();
        }
    }
    else
    {
        DF3D_ASSERT(data.mipLevels.size() == 1);
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat,
                              width, height, 0, pixelDataFormat,
                              GL_UNSIGNED_BYTE, data.mipLevels[0].pixels.data()));

        bool mipmapped = false;
        if (((flags & TEXTURE_FILTERING_MASK) == TEXTURE_FILTERING_TRILINEAR) ||
            ((flags & TEXTURE_FILTERING_MASK) == TEXTURE_FILTERING_ANISOTROPIC))
        {
            // Generate mip maps if not provided with texture.
            if (data.mipLevels.size() == 1)
            {
                GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
                mipmapped = true;
            }
        }

        m_sizeInBytes = GetTextureSize(glInternalFormat, width, height, mipmapped);
    }

    if (maxAniso > 0.0f)
    {
        if ((flags & TEXTURE_FILTERING_MASK) == TEXTURE_FILTERING_ANISOTROPIC)
            GL_CHECK(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, std::max(1.0f, maxAniso)));
    }

    m_glType = GL_TEXTURE_2D;
    m_glPixelFormat = pixelDataFormat;
    m_pixelFmt = data.format;

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

    if (data.format == PixelFormat::KTX)
    {
        if (previousUnpackAlignment != 4)
            glPixelStorei(GL_UNPACK_ALIGNMENT, previousUnpackAlignment);
    }

    return true;
}

void GLTexture::destroy()
{
    if (m_glID)
    {
        GL_CHECK(glBindTexture(m_glType, 0));
        GL_CHECK(glDeleteTextures(1, &m_glID));
    }
    m_glID = 0;
}

void GLTexture::updateData(int originX, int originY, int width, int height, const void *data)
{
    // FIXME: works only for 2D textures.
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_glID));
    GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, originX, originY, width, height, m_glPixelFormat, GL_UNSIGNED_BYTE, data));
}

void GLTexture::bindTexture(int unit)
{
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + unit));
    GL_CHECK(glBindTexture(m_glType, m_glID));
}

VertexBufferHandle RenderBackendGL::createVBHelper(const VertexFormat &format, uint32_t numVertices, const void *data, bool dynamic)
{
    VertexBufferHandle vbHandle;
    GLVertexBuffer vbuffer;

    if (vbuffer.init(format, numVertices, data, dynamic))
    {
        vbHandle.set(m_vertexBuffersBag.getNew());

        m_vertexBuffers[vbHandle.getIndex()] = vbuffer;

        TRACE_GPU_ALLOC("Vertex Buffer", vbuffer.getSize());
    }

    return vbHandle;
}

GLuint RenderBackendGL::createShader(const char *data, GLenum type)
{
    GLuint shaderID = 0;

    if (!data)
    {
        DFLOG_WARN("Failed to create a shader: empty shader data");
        return shaderID;
    }

    GL_CHECK(shaderID = glCreateShader(type));

    if (shaderID == 0)
    {
        DFLOG_WARN("Failed to create a shader");
        return shaderID;
    }

    // Compile the shader.
    GL_CHECK(glShaderSource(shaderID, 1, &data, nullptr));
    GL_CHECK(glCompileShader(shaderID));

#ifdef _DEBUG
    int compileOk;
    GL_CHECK(glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileOk));
    if (compileOk == GL_FALSE)
    {
        DFLOG_WARN("Failed to compile a shader");
        DFLOG_MESS("\n\n%s\n\n", data);
        PrintShaderLog(shaderID);

        GL_CHECK(glDeleteShader(shaderID));

        DEBUG_BREAK();
    }
#endif

    return shaderID;
}

void RenderBackendGL::destroyShader(GLuint programID, GLuint shaderID)
{
    GL_CHECK(glDetachShader(programID, shaderID));
    GL_CHECK(glDeleteShader(shaderID));
}

void RenderBackendGL::requestUniforms(GLProgram &programGL)
{
    DF3D_ASSERT(programGL.uniforms.empty());

    int total = 0;
    GL_CHECK(glGetProgramiv(programGL.glID, GL_ACTIVE_UNIFORMS, &total));

    for (int i = 0; i < total; i++)
    {
        GLenum type = GL_INVALID_ENUM;
        int nameLength = -1, uniformVarSize = -1;
        char name[256];

        GL_CHECK(glGetActiveUniform(programGL.glID, i, sizeof(name) - 1, &nameLength, &uniformVarSize, &type, name));
        name[nameLength] = 0;

        GLProgram::Uniform uniform;
        uniform.type = type;
        uniform.name = name;
        GL_CHECK(uniform.location = glGetUniformLocation(programGL.glID, name));

        programGL.uniforms.push_back(uniform);
    }
}

void RenderBackendGL::setupDepthTest(uint64_t depthState)
{
    if (depthState)
    {
        GL_CHECK(glEnable(GL_DEPTH_TEST));

        DF3D_ASSERT(depthState <= RENDER_STATE_DEPTH_ALWAYS);
        GL_CHECK(glDepthFunc(g_depthFuncLookup[depthState]));
    }
    else
    {
        GL_CHECK(glDisable(GL_DEPTH_TEST));
    }
}

void RenderBackendGL::setupDepthWrite(uint64_t dwState)
{
    GL_CHECK(glDepthMask(dwState == RENDER_STATE_DEPTH_WRITE ? GL_TRUE : GL_FALSE));
}

void RenderBackendGL::setupFaceCulling(uint64_t faceState)
{
    if (faceState == RENDER_STATE_FRONT_FACE_CW)
    {
        GL_CHECK(glEnable(GL_CULL_FACE));
        GL_CHECK(glFrontFace(GL_CW));
    }
    else if (faceState == RENDER_STATE_FRONT_FACE_CCW)
    {
        GL_CHECK(glEnable(GL_CULL_FACE));
        GL_CHECK(glFrontFace(GL_CCW));
    }
    else
    {
        GL_CHECK(glDisable(GL_CULL_FACE));
    }
}

void RenderBackendGL::setupBlending(uint64_t srcFactor, uint64_t dstFactor)
{
    if (srcFactor == 0 && dstFactor == 0)
    {
        GL_CHECK(glDisable(GL_BLEND));
    }
    else
    {
        GL_CHECK(glEnable(GL_BLEND));

        DF3D_ASSERT(srcFactor >= RENDER_STATE_BLEND_ONE && srcFactor <= RENDER_STATE_BLEND_ONE_MINUS_SRC_ALPHA);
        DF3D_ASSERT(dstFactor >= RENDER_STATE_BLEND_ONE && dstFactor <= RENDER_STATE_BLEND_ONE_MINUS_SRC_ALPHA);
        GL_CHECK(glBlendFunc(g_blendFuncLookup[srcFactor], g_blendFuncLookup[dstFactor]));
    }
}

RenderBackendGL::RenderBackendGL(int width, int height)
    : m_vertexBuffersBag(MemoryManager::allocDefault()),
    m_indexBuffersBag(MemoryManager::allocDefault()),
    m_texturesBag(MemoryManager::allocDefault()),
    m_gpuProgramsBag(MemoryManager::allocDefault())
{
#ifdef DF3D_DESKTOP
    // Init GLEW.
    glewExperimental = GL_TRUE;

    auto glewerr = glewInit();
    if (glewerr != GLEW_OK)
    {
        std::string errStr = "GLEW initialization failed: ";
        errStr += (const char *)glewGetErrorString(glewerr);
        throw std::runtime_error(errStr);
    }

    if (!glewIsSupported("GL_VERSION_2_1"))
        throw std::runtime_error("GL 2.1 unsupported");
#endif

    // Init extensions.
    const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
    glGetError();

    m_extensionsString = extensions;
    m_anisotropicFilteringSupported = m_extensionsString.find("GL_EXT_texture_filter_anisotropic") != std::string::npos;

    GL_CHECK(glDepthFunc(GL_LEQUAL));
    GL_CHECK(glDisable(GL_DEPTH_TEST));
    GL_CHECK(glDepthMask(GL_FALSE));
    GL_CHECK(glFrontFace(GL_CCW));
    GL_CHECK(glDisable(GL_SCISSOR_TEST));
    GL_CHECK(glDisable(GL_CULL_FACE));
    GL_CHECK(glDisable(GL_BLEND));
    GL_CHECK(glCullFace(GL_BACK));

    GL_CHECK(glPixelStorei(GL_PACK_ALIGNMENT, 1));
    GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    GL_CHECK(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_caps.maxTextureSize));
    if (m_anisotropicFilteringSupported)
        GL_CHECK(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &m_caps.maxAnisotropy));

    // WORKAROUND
    if (m_caps.maxTextureSize < 2048)
        throw std::runtime_error("Hardware not supported");

#ifdef _DEBUG
    // Print GPU info.
    const char *ver = (const char *)glGetString(GL_VERSION);
    DFLOG_MESS("OpenGL version %s", ver);

    const char *card = (const char *)glGetString(GL_RENDERER);
    const char *vendor = (const char *)glGetString(GL_VENDOR);
    DFLOG_MESS("Using %s %s", card, vendor);

    const char *shaderVer = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    DFLOG_MESS("Shaders version %s", shaderVer);

    DFLOG_MESS("Max texture size %d", m_caps.maxTextureSize);

    DFLOG_DEBUG("RenderBackendGL storage %d KB", utils::sizeKB(sizeof(*this)));
#endif

#if defined(DF3D_DESKTOP)
    glEnable(GL_MULTISAMPLE);
#endif
}

RenderBackendGL::~RenderBackendGL()
{
    DF3D_ASSERT(m_vertexBuffersBag.empty());
    DF3D_ASSERT(m_indexBuffersBag.empty());
    DF3D_ASSERT(m_texturesBag.empty());
    DF3D_ASSERT(m_gpuProgramsBag.empty());
}

RenderBackendCaps RenderBackendGL::getCaps()
{
    return m_caps;
}

FrameStats RenderBackendGL::getLastFrameStats()
{
    m_frameStats.gpuMemBytes = g_memStats.getMemTotal();
    return m_frameStats;
}

void RenderBackendGL::frameBegin()
{
    GL_CHECK(glDisable(GL_SCISSOR_TEST));

    GL_CHECK(glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a));
    GL_CHECK(glClearDepthf(m_clearDepth));

    GL_CHECK(glDepthMask(GL_TRUE)); // Depth write should be enabled before clearing depth buffer.

    // Clear the buffers.
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

    // Everything about m_pipeLineState.state is 0 and so disabled.
    GL_CHECK(glDepthFunc(GL_LEQUAL));
    GL_CHECK(glDisable(GL_DEPTH_TEST));
    GL_CHECK(glDisable(GL_CULL_FACE));
    GL_CHECK(glDepthMask(GL_FALSE));
    GL_CHECK(glDisable(GL_BLEND));

    GL_CHECK(glUseProgram(0));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));

    m_frameStats.drawCalls = m_frameStats.totalLines = m_frameStats.totalTriangles = 0;

    m_pipeLineState = {};
}

void RenderBackendGL::frameEnd()
{

}

VertexBufferHandle RenderBackendGL::createStaticVertexBuffer(const VertexFormat &format, uint32_t numVertices, const void *data)
{
    return createVBHelper(format, numVertices, data, false);
}

VertexBufferHandle RenderBackendGL::createDynamicVertexBuffer(const VertexFormat &format, uint32_t numVertices, const void *data)
{
    return createVBHelper(format, numVertices, data, true);
}

void RenderBackendGL::destroyVertexBuffer(VertexBufferHandle handle)
{
    if (m_vertexBuffersBag.isValid(handle.getID()))
    {
        auto &vbuffer = m_vertexBuffers[handle.getIndex()];

        TRACE_GPU_FREE("Vertex Buffer", vbuffer.getSize());

        if (!m_destroyAndroidWorkaround)
            vbuffer.destroy();

        vbuffer = {};
        m_vertexBuffersBag.release(handle.getID());
    }
    else
        DF3D_ASSERT(false);
}

void RenderBackendGL::bindVertexBuffer(VertexBufferHandle handle, uint32_t vertexStart)
{
    if (m_vertexBuffersBag.isValid(handle.getID()))
    {
        auto &vertexBuffer = m_vertexBuffers[handle.getIndex()];

        vertexBuffer.bindBuffer(vertexStart);

        m_pipeLineState.indexedDrawCall = false;
    }
    else
        DF3D_ASSERT(false);
}

void RenderBackendGL::updateVertexBuffer(VertexBufferHandle handle, uint32_t vertexStart, uint32_t numVertices, const void *data)
{
    if (m_vertexBuffersBag.isValid(handle.getID()))
    {
        auto &vertexBuffer = m_vertexBuffers[handle.getIndex()];

        vertexBuffer.updateData(vertexStart, numVertices, data);
    }
    else
        DF3D_ASSERT(false);
}

IndexBufferHandle RenderBackendGL::createIndexBuffer(uint32_t numIndices, const void *data, IndicesType indicesType)
{
    // NOTE: some GPUs do not support 32-bit indices (Mali 400)
    DF3D_ASSERT(indicesType == INDICES_16_BIT);
    DF3D_ASSERT(numIndices > 0);

    IndexBufferHandle ibHandle;
    GLIndexBuffer ibuffer;

    if (ibuffer.init(numIndices, data, indicesType == INDICES_16_BIT))
    {
        ibHandle.set(m_indexBuffersBag.getNew());

        m_indexBuffers[ibHandle.getIndex()] = ibuffer;

        TRACE_GPU_ALLOC("Index Buffer", ibuffer.getSize());
    }

    return ibHandle;
}

void RenderBackendGL::destroyIndexBuffer(IndexBufferHandle handle)
{
    if (m_indexBuffersBag.isValid(handle.getID()))
    {
        auto &indexBuffer = m_indexBuffers[handle.getIndex()];

        TRACE_GPU_FREE("Index Buffer", indexBuffer.getSize());

        if (!m_destroyAndroidWorkaround)
            indexBuffer.destroy();

        indexBuffer = {};

        m_indexBuffersBag.release(handle.getID());
    }
    else
        DF3D_ASSERT(false);
}

void RenderBackendGL::bindIndexBuffer(IndexBufferHandle handle)
{
    if (m_indexBuffersBag.isValid(handle.getID()))
    {
        auto &indexBuffer = m_indexBuffers[handle.getIndex()];

        indexBuffer.bindBuffer();

        m_pipeLineState.indexedDrawCall = true;
        m_pipeLineState.currIndicesType = indexBuffer.is16Bit() ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
    }
    else
        DF3D_ASSERT(false);
}

TextureHandle RenderBackendGL::createTexture(const TextureResourceData &data, uint32_t flags)
{
    TextureHandle textureHandle;
    GLTexture texture;

    if (texture.init(data, flags, m_caps.maxTextureSize, m_anisotropicFilteringSupported ? m_caps.maxAnisotropy : 0.0f))
    {
        textureHandle.set(m_texturesBag.getNew());

        m_textures[textureHandle.getIndex()] = texture;

        m_frameStats.textures++;
        TRACE_GPU_ALLOC("Texture", texture.getSize());
    }

    return textureHandle;
}

void RenderBackendGL::updateTexture(TextureHandle handle, int originX, int originY, int width, int height, const void *data)
{
    if (m_texturesBag.isValid(handle.getID()))
    {
        m_textures[handle.getIndex()].updateData(originX, originY, width, height, data);
    }
    else
        DF3D_ASSERT(false);
}

void RenderBackendGL::destroyTexture(TextureHandle handle)
{
    if (m_texturesBag.isValid(handle.getID()))
    {
        auto &texture = m_textures[handle.getIndex()];

        DF3D_ASSERT(m_frameStats.textures > 0);
        m_frameStats.textures--;
        TRACE_GPU_FREE("Texture", texture.getSize());

        if (!m_destroyAndroidWorkaround)
            texture.destroy();

        texture = {};

        m_texturesBag.release(handle.getID());
    }
    else
        DF3D_ASSERT(false);
}

void RenderBackendGL::bindTexture(GPUProgramHandle program, TextureHandle handle, UniformHandle textureUniform, int unit)
{
    if (m_texturesBag.isValid(handle.getID()))
    {
        m_textures[handle.getIndex()].bindTexture(unit);

        setUniformValue(program, textureUniform, &unit);
    }
    else
        DF3D_ASSERT(false);
}

GPUProgramHandle RenderBackendGL::createGPUProgram(const char *vertexShaderData, const char *fragmentShaderData)
{
    GPUProgramHandle handle;
    GLProgram program;

    GL_CHECK(program.glID = glCreateProgram());
    if (program.glID == 0)
    {
        DFLOG_WARN("Failed to create a GPU program");
        return handle;
    }

    auto vShaderID = createShader(vertexShaderData, GL_VERTEX_SHADER);
    auto fShaderID = createShader(fragmentShaderData, GL_FRAGMENT_SHADER);

    GL_CHECK(glAttachShader(program.glID, vShaderID));
    GL_CHECK(glAttachShader(program.glID, fShaderID));

    // TODO: use VAO + refactor this.
    GL_CHECK(glBindAttribLocation(program.glID, VertexFormat::POSITION, "a_vertex3"));
    GL_CHECK(glBindAttribLocation(program.glID, VertexFormat::NORMAL, "a_normal"));
    GL_CHECK(glBindAttribLocation(program.glID, VertexFormat::TX, "a_txCoord"));
    GL_CHECK(glBindAttribLocation(program.glID, VertexFormat::COLOR, "a_vertexColor"));
    GL_CHECK(glBindAttribLocation(program.glID, VertexFormat::TANGENT, "a_tangent"));
    GL_CHECK(glBindAttribLocation(program.glID, VertexFormat::BITANGENT, "a_bitangent"));

    GL_CHECK(glLinkProgram(program.glID));

#ifdef _DEBUG
    int linkOk;
    GL_CHECK(glGetProgramiv(program.glID, GL_LINK_STATUS, &linkOk));
    if (linkOk == GL_FALSE)
    {
        DFLOG_WARN("GPU program linkage failed");
        PrintGPUProgramLog(program.glID);

        DEBUG_BREAK();
    }
#endif

    GL_CHECK(glUseProgram(0));

    requestUniforms(program);

    // Must detach shaders only after uniforms were retreived.
    // NVIDIA Tegra crash workaround!
    destroyShader(program.glID, vShaderID);
    destroyShader(program.glID, fShaderID);

    handle.set(m_gpuProgramsBag.getNew());
    m_gpuPrograms[handle.getIndex()] = program;

    return handle;
}

void RenderBackendGL::destroyGPUProgram(GPUProgramHandle handle)
{
    if (m_gpuProgramsBag.isValid(handle.getID()))
    {
        auto &programGL = m_gpuPrograms[handle.getIndex()];

        if (!m_destroyAndroidWorkaround)
        {
            GL_CHECK(glUseProgram(0));
            GL_CHECK(glDeleteProgram(programGL.glID));
        }

        programGL = {};
        m_gpuProgramsBag.release(handle.getID());
    }
    else
        DF3D_ASSERT(false);
}

void RenderBackendGL::bindGPUProgram(GPUProgramHandle handle)
{
    if (handle == m_pipeLineState.program)
        return;

    if (m_gpuProgramsBag.isValid(handle.getID()))
    {
        const auto &programGL = m_gpuPrograms[handle.getIndex()];
        DF3D_ASSERT(programGL.glID != 0);

        GL_CHECK(glUseProgram(programGL.glID));

        m_pipeLineState.program = handle;
    }
    else
        DF3D_ASSERT(false);
}

UniformHandle RenderBackendGL::getUniform(GPUProgramHandle program, const char *name)
{
    if (m_gpuProgramsBag.isValid(program.getID()))
    {
        const auto &pr = m_gpuPrograms[program.getIndex()];
        std::string strName = name;

        for (size_t i = 0; i < pr.uniforms.size(); i++)
        {
            if (pr.uniforms[i].name == strName)
                return UniformHandle{ i + 1 };  // Zero is reserved for invalid handle.
        }
    }
    else
        DF3D_ASSERT(false);

    return {};
}

void RenderBackendGL::setUniformValue(GPUProgramHandle program, UniformHandle uniformHandle, const void *data)
{
    const auto &programGL = m_gpuPrograms[program.getIndex()];
    const auto &uniformGL = programGL.uniforms[uniformHandle.getID() - 1];

    DF3D_ASSERT(uniformGL.type != GL_INVALID_ENUM && uniformGL.location != -1);

    switch (uniformGL.type)
    {
    case GL_SAMPLER_2D:
        GL_CHECK(glUniform1iv(uniformGL.location, 1, (GLint *)data));
        break;
    case GL_SAMPLER_CUBE:
        GL_CHECK(glUniform1iv(uniformGL.location, 1, (GLint *)data));
        break;
    case GL_INT:
        GL_CHECK(glUniform1iv(uniformGL.location, 1, (GLint *)data));
        break;
    case GL_FLOAT:
        GL_CHECK(glUniform1fv(uniformGL.location, 1, (GLfloat *)data));
        break;
    case GL_FLOAT_VEC2:
        GL_CHECK(glUniform2fv(uniformGL.location, 1, (GLfloat *)data));
        break;
    case GL_FLOAT_VEC3:
        GL_CHECK(glUniform3fv(uniformGL.location, 1, (GLfloat *)data));
        break;
    case GL_FLOAT_VEC4:
        GL_CHECK(glUniform4fv(uniformGL.location, 1, (GLfloat *)data));
        break;
    case GL_FLOAT_MAT3:
        GL_CHECK(glUniformMatrix3fv(uniformGL.location, 1, GL_FALSE, (GLfloat *)data));
        break;
    case GL_FLOAT_MAT4:
        GL_CHECK(glUniformMatrix4fv(uniformGL.location, 1, GL_FALSE, (GLfloat *)data));
        break;
    default:
        DFLOG_WARN("Failed to update GpuProgramUniform. Unknown uniform type");
        break;
    }
}

void RenderBackendGL::setViewport(const Viewport &viewport)
{
    GL_CHECK(glViewport(viewport.originX, viewport.originY, viewport.width, viewport.height));
}

void RenderBackendGL::setScissorTest(bool enabled, const Viewport &rect)
{
    if (enabled)
    {
        GL_CHECK(glEnable(GL_SCISSOR_TEST));
        GL_CHECK(glScissor(rect.originX, rect.originY, rect.width, rect.height));
    }
    else
    {
        GL_CHECK(glDisable(GL_SCISSOR_TEST));
    }
}

void RenderBackendGL::setClearData(const glm::vec3 &color, float depth)
{
    m_clearColor = glm::vec4(color, 1.0f);
    m_clearDepth = depth;
}

void RenderBackendGL::setState(uint64_t state)
{
    if (state == m_pipeLineState.state)
        return;

    setupDepthTest(state & RENDER_STATE_DEPTH_MASK);
    setupDepthWrite(state & RENDER_STATE_DEPTH_WRITE_MASK);
    setupFaceCulling(state & RENDER_STATE_FACE_CULL_MASK);
    setupBlending(GetBlendingSrcFactor(state), GetBlendingDstFactor(state));

    m_pipeLineState.state = state;
}

void RenderBackendGL::draw(Topology type, uint32_t numberOfElements)
{
    if (m_pipeLineState.indexedDrawCall)
        GL_CHECK(glDrawElements(GetGLDrawMode(type), numberOfElements, m_pipeLineState.currIndicesType, nullptr));
    else
        GL_CHECK(glDrawArrays(GetGLDrawMode(type), 0, numberOfElements));

    // Update stats.
#ifdef _DEBUG
    {
        m_frameStats.drawCalls++;
        switch (type)
        {
        case Topology::LINES:
            m_frameStats.totalLines += numberOfElements / 2;
            break;
        case Topology::TRIANGLES:
            m_frameStats.totalTriangles += numberOfElements / 3;
            break;
        case Topology::TRIANGLE_STRIP:
            if (numberOfElements >= 3)
                m_frameStats.totalTriangles += numberOfElements - 2;
            break;
        default:
            break;
        }
    }
#endif
}

}
