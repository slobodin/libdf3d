#include "RenderBackendGL.h"

#include <df3d/engine/render/RenderCommon.h>
#include <df3d/engine/render/Texture.h>
#include <df3d/engine/render/RenderOperation.h>
#include <df3d/engine/render/GpuProgram.h>
#include <df3d/lib/Utils.h>

namespace df3d {

static const char* GLErrorCodeToString(GLenum errcode)
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

#if defined(_DEBUG) || defined(DEBUG)
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

#if defined(_DEBUG) || defined(DEBUG)
#define HANDLE_CHECK(handle) do { if (!handle.valid()) { DFLOG_WARN("Invalid handle"); DEBUG_BREAK(); return; } } while (0)
#else
#define HANDLE_CHECK(handle) do { if (!handle.valid()) return; } while (0)
#endif

#if defined(_DEBUG) || defined(DEBUG)
#define HANDLE_CHECK_RETURN_INVALID(handle) do { if (!handle.valid()) { DFLOG_WARN("Invalid handle"); return {}; } } while (0)
#else
#define HANDLE_CHECK_RETURN_INVALID(handle) do { if (!handle.valid()) return{}; } while (0)
#endif

static bool IsPot(size_t v)
{
    return v && !(v & (v - 1));
}

static size_t GetNextPot(size_t v)
{
    if (!IsPot(v))
    {
        int n = 0;
        while (v >>= 1)
            ++n;

        v = 1 << (n + 1);
    }
    return v;
}

static const std::map<CubeFace, GLenum> MapSidesToGL =
{
    { CubeFace::POSITIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_X },
    { CubeFace::NEGATIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X },
    { CubeFace::POSITIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Y },
    { CubeFace::NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y },
    { CubeFace::POSITIVE_Z, GL_TEXTURE_CUBE_MAP_POSITIVE_Z },
    { CubeFace::NEGATIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z }
};

static GLint GetGLFilteringMode(TextureFiltering filtering, bool mipmapped)
{
    switch (filtering)
    {
    case TextureFiltering::NEAREST:
        return !mipmapped ? GL_NEAREST : GL_NEAREST_MIPMAP_NEAREST;
    case TextureFiltering::BILINEAR:
        return !mipmapped ? GL_LINEAR : GL_LINEAR_MIPMAP_NEAREST;
    case TextureFiltering::TRILINEAR:
        return !mipmapped ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR;
    default:
        break;
    }

    return -1;
}

static GLint GetGLWrapMode(TextureWrapMode mode)
{
    switch (mode)
    {
    case TextureWrapMode::WRAP:
        return GL_REPEAT;
    case TextureWrapMode::CLAMP:
        return GL_CLAMP_TO_EDGE;
    default:
        break;
    }

    return -1;
}


static void SetupGLTextureFiltering(GLenum glType, TextureFiltering filtering, bool mipmapped)
{
    GL_CHECK(glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, filtering == TextureFiltering::NEAREST ? GL_NEAREST : GL_LINEAR));
    GL_CHECK(glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GetGLFilteringMode(filtering, mipmapped)));
}

static void SetupGLWrapMode(GLenum glType, TextureWrapMode wrapMode)
{
    auto wmGl = GetGLWrapMode(wrapMode);
    GL_CHECK(glTexParameteri(glType, GL_TEXTURE_WRAP_S, wmGl));
    GL_CHECK(glTexParameteri(glType, GL_TEXTURE_WRAP_T, wmGl));
#if defined(DF3D_DESKTOP)
    GL_CHECK(glTexParameteri(glType, GL_TEXTURE_WRAP_R, wmGl));
#endif
}

static GLenum GetGLBufferUsageType(GpuBufferUsageType t)
{
    switch (t)
    {
    case GpuBufferUsageType::STATIC:
        return GL_STATIC_DRAW;
    case GpuBufferUsageType::DYNAMIC:
        return GL_DYNAMIC_DRAW;
    case GpuBufferUsageType::STREAM:
        return GL_STREAM_DRAW;
    default:
        break;
    }

    return GL_INVALID_ENUM;
}

static GLenum GetGLDrawMode(Topology type)
{
    switch (type)
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
static void PrintShaderLog(GLuint shader)
{
    int infologLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);

    unique_ptr<char[]> infoLog(new char[infologLen + 1]);
    glGetShaderInfoLog(shader, infologLen, nullptr, infoLog.get());

    DFLOG_MESS("Shader info log: %s", infoLog.get());
}

static void PrintGpuProgramLog(unsigned int program)
{
    int infologLen = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLen);

    unique_ptr<char[]> infoLog(new char[infologLen + 1]);
    glGetProgramInfoLog(program, infologLen, nullptr, infoLog.get());

    DFLOG_MESS("GPU program info log: %s", infoLog.get());
}
#endif

void RenderBackendGL::initExtensions()
{
    const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
    glGetError();

    m_extensionsString = extensions;
    m_anisotropicFilteringSupported = m_extensionsString.find("GL_EXT_texture_filter_anisotropic") != std::string::npos;
}

void RenderBackendGL::destroyShader(ShaderHandle shader, GLuint programId)
{
    HANDLE_CHECK(shader);

    const auto &shaderGL = m_shaders[shader.id];
    DF3D_ASSERT(shaderGL.gl_id != 0);

    GL_CHECK(glDetachShader(programId, shaderGL.gl_id));
    GL_CHECK(glDeleteShader(shaderGL.gl_id));

    m_shaders[shader.id] = {};

    m_shadersBag.release(shader.id);
}

RenderBackendGL::RenderBackendGL(int width, int height)
    : m_width(width),
    m_height(height)
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

    initialize();
}

RenderBackendGL::~RenderBackendGL()
{

}

const RenderBackendCaps& RenderBackendGL::getCaps() const
{
    return m_caps;
}

const FrameStats& RenderBackendGL::getFrameStats() const
{
#ifdef _DEBUG
    m_stats.gpuMemBytes = m_gpuMemStats.getGpuMemBytes();
#endif
    return m_stats;
}

void RenderBackendGL::initialize()
{
    m_vertexBuffersBag = {};
    m_indexBuffersBag = {};
    m_texturesBag = {};
    m_shadersBag = {};
    m_gpuProgramsBag = {};
    m_uniformsBag = {};

    m_programUniforms.clear();

    m_drawState = {};
    m_indexedDrawCall = false;
    m_currentIndexType = GL_INVALID_ENUM;

    initExtensions();

    std::fill(std::begin(m_vertexBuffers), std::end(m_vertexBuffers), VertexBufferGL());
    std::fill(std::begin(m_indexBuffers), std::end(m_indexBuffers), IndexBufferGL());
    std::fill(std::begin(m_textures), std::end(m_textures), TextureGL());
    std::fill(std::begin(m_shaders), std::end(m_shaders), ShaderGL());
    std::fill(std::begin(m_programs), std::end(m_programs), ProgramGL());
    std::fill(std::begin(m_uniforms), std::end(m_uniforms), UniformGL());

    GL_CHECK(glDepthFunc(GL_LEQUAL));
    GL_CHECK(glDisable(GL_DEPTH_TEST));
    GL_CHECK(glDepthMask(GL_FALSE));
    GL_CHECK(glFrontFace(GL_CCW));
    GL_CHECK(glDisable(GL_SCISSOR_TEST));
    GL_CHECK(glDisable(GL_CULL_FACE));
    GL_CHECK(glDisable(GL_BLEND));

    GL_CHECK(glPixelStorei(GL_PACK_ALIGNMENT, 1));
    GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    GL_CHECK(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_caps.maxTextureSize));
    if (m_anisotropicFilteringSupported)
        GL_CHECK(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &m_caps.maxAnisotropy));

    // Print GPU info.
    {
        const char *ver = (const char *)glGetString(GL_VERSION);
        DFLOG_MESS("OpenGL version %s", ver);

        const char *card = (const char *)glGetString(GL_RENDERER);
        const char *vendor = (const char *)glGetString(GL_VENDOR);
        DFLOG_MESS("Using %s %s", card, vendor);

        const char *shaderVer = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
        DFLOG_MESS("Shaders version %s", shaderVer);

        DFLOG_MESS("Max texture size %d", m_caps.maxTextureSize);
    }

#ifdef _DEBUG
    size_t totalStorage = sizeof(m_vertexBuffers) +
        sizeof(m_indexBuffers) +
        sizeof(m_textures) +
        sizeof(m_shaders) +
        sizeof(m_programs) +
        sizeof(m_uniforms);

    DFLOG_DEBUG("RenderBackendGL storage %d KB", utils::sizeKB(totalStorage));
#endif

#if defined(DF3D_DESKTOP)
    glEnable(GL_MULTISAMPLE);
#endif
}

void RenderBackendGL::frameBegin()
{
    m_stats.drawCalls = m_stats.totalLines = m_stats.totalTriangles = 0;

    m_vertexBuffersBag.cleanup();
    m_indexBuffersBag.cleanup();
    m_texturesBag.cleanup();
    m_shadersBag.cleanup();
    m_gpuProgramsBag.cleanup();
    m_uniformsBag.cleanup();

    m_indexedDrawCall = false;
    m_currentIndexType = GL_INVALID_ENUM;
    m_currentProgram.invalidate();
    m_currentVertexBuffer.invalidate();
    m_currentIndexBuffer.invalidate();

    GL_CHECK(glUseProgram(0));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

void RenderBackendGL::frameEnd()
{

}

VertexBufferHandle RenderBackendGL::createVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data, GpuBufferUsageType usage)
{
    DF3D_ASSERT(verticesCount > 0);

    VertexBufferHandle vbHandle = m_vertexBuffersBag.getNew();
    if (!vbHandle.valid())
    {
        DFLOG_WARN("Failed to create a vertex buffer handle.");
        return{};
    }

    VertexBufferGL vertexBuffer;

    vertexBuffer.format = format;
    vertexBuffer.sizeInBytes = verticesCount * format.getVertexSize();

    GL_CHECK(glGenBuffers(1, &vertexBuffer.gl_id));
    if (vertexBuffer.gl_id == 0)
    {
        DFLOG_WARN("Failed to create a GL vertex buffer.");
        return{};
    }

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.gl_id));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertexBuffer.sizeInBytes, data, GetGLBufferUsageType(usage)));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));

    m_vertexBuffers[vbHandle.id] = vertexBuffer;

    m_currentVertexBuffer.invalidate();

#ifdef _DEBUG
    m_gpuMemStats.addVertexBuffer(vbHandle, vertexBuffer.sizeInBytes);
#endif

    return vbHandle;
}

void RenderBackendGL::destroyVertexBuffer(VertexBufferHandle vbHandle)
{
    HANDLE_CHECK(vbHandle);

    const auto &vertexBuffer = m_vertexBuffers[vbHandle.id];

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));

    if (vertexBuffer.gl_id)
        GL_CHECK(glDeleteBuffers(1, &vertexBuffer.gl_id));
    else
        return;

    m_vertexBuffers[vbHandle.id] = {};
    m_vertexBuffersBag.release(vbHandle.id);
    m_currentVertexBuffer.invalidate();

#ifdef _DEBUG
    m_gpuMemStats.removeVertexBuffer(vbHandle);
#endif
}

void RenderBackendGL::bindVertexBuffer(VertexBufferHandle vbHandle)
{
    HANDLE_CHECK(vbHandle);

    // FIXME:
    //if (m_currentVertexBuffer.valid() && vb.id == m_currentVertexBuffer.id)
    //    return;

    const auto &vertexBuffer = m_vertexBuffers[vbHandle.id];
    DF3D_ASSERT(vertexBuffer.gl_id != 0);

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.gl_id));

    const auto &format = vertexBuffer.format;
    const auto vertexSize = format.getVertexSize();

    for (uint16_t i = VertexFormat::POSITION_3; i != VertexFormat::COUNT; i++)
    {
        auto attrib = (VertexFormat::VertexAttribute)i;

        if (!format.hasAttribute(attrib))
            continue;

        GL_CHECK(glEnableVertexAttribArray(attrib));
        size_t offset = format.getOffsetTo(attrib);
        size_t count = format.getCompCount(attrib);

        GL_CHECK(glVertexAttribPointer(attrib, count, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*)offset));
    }

    m_indexedDrawCall = false;
    m_currentVertexBuffer = vbHandle;
}

void RenderBackendGL::updateVertexBuffer(VertexBufferHandle vbHandle, size_t verticesCount, const void *data)
{
    HANDLE_CHECK(vbHandle);

    const auto &vertexBuffer = m_vertexBuffers[vbHandle.id];
    DF3D_ASSERT(vertexBuffer.gl_id != 0);

    auto bytesUpdating = verticesCount * vertexBuffer.format.getVertexSize();

    DF3D_ASSERT(bytesUpdating <= vertexBuffer.sizeInBytes);
    DF3D_ASSERT(vertexBuffer.gl_id != 0);

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.gl_id));
    GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, bytesUpdating, data));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));

    m_currentVertexBuffer.invalidate();
}

IndexBufferHandle RenderBackendGL::createIndexBuffer(size_t indicesCount, const void *data, GpuBufferUsageType usage, IndicesType indicesType)
{
    DF3D_ASSERT(indicesCount > 0);

    IndexBufferHandle ibHandle = m_indexBuffersBag.getNew();
    if (!ibHandle.valid())
    {
        DFLOG_WARN("Failed to create an index buffer");
        return{};
    }

    IndexBufferGL indexBuffer;

    GL_CHECK(glGenBuffers(1, &indexBuffer.gl_id));
    if (indexBuffer.gl_id == 0)
        return{};

    indexBuffer.indices16bit = (indicesType == INDICES_16_BIT);

    indexBuffer.sizeInBytes = indicesCount * (indexBuffer.indices16bit ? sizeof(uint16_t) : sizeof(uint32_t));

    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.gl_id));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.sizeInBytes, data, GetGLBufferUsageType(usage)));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    m_indexBuffers[ibHandle.id] = indexBuffer;
    m_currentIndexBuffer.invalidate();

#ifdef _DEBUG
    m_gpuMemStats.addIndexBuffer(ibHandle, indexBuffer.sizeInBytes);
#endif

    return ibHandle;
}

void RenderBackendGL::destroyIndexBuffer(IndexBufferHandle ibHandle)
{
    HANDLE_CHECK(ibHandle);

    const auto &indexBuffer = m_indexBuffers[ibHandle.id];

    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    if (indexBuffer.gl_id != 0)
        GL_CHECK(glDeleteBuffers(1, &indexBuffer.gl_id));
    else
        return;

    m_indexBuffers[ibHandle.id] = {};
    m_indexBuffersBag.release(ibHandle.id);
    m_currentIndexBuffer.invalidate();

#ifdef _DEBUG
    m_gpuMemStats.removeIndexBuffer(ibHandle);
#endif
}

void RenderBackendGL::bindIndexBuffer(IndexBufferHandle ib)
{
    HANDLE_CHECK(ib);

    //if (m_currentIndexBuffer.valid() && ib.id == m_currentIndexBuffer.id)
    //    return;

    const auto &indexBuffer = m_indexBuffers[ib.id];
    DF3D_ASSERT(indexBuffer.gl_id != 0);

    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.gl_id));

    m_indexedDrawCall = true;
    m_currentIndexType = indexBuffer.indices16bit ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
    m_currentIndexBuffer = ib;
}

void RenderBackendGL::updateIndexBuffer(IndexBufferHandle ib, size_t indicesCount, const void *data)
{
    HANDLE_CHECK(ib);

    const auto &indexBuffer = m_indexBuffers[ib.id];
    DF3D_ASSERT(indexBuffer.gl_id != 0);

    auto bytesUpdating = indicesCount * (indexBuffer.indices16bit ? sizeof(uint16_t) : sizeof(uint32_t));

    DF3D_ASSERT(bytesUpdating <= indexBuffer.sizeInBytes);
    DF3D_ASSERT(indexBuffer.gl_id != 0);

    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.gl_id));
    GL_CHECK(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bytesUpdating, data));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    m_currentIndexBuffer.invalidate();
}

TextureHandle RenderBackendGL::createTexture2D(int width, int height, PixelFormat format, const uint8_t *data, const TextureCreationParams &params)
{
    TextureHandle textureHandle = m_texturesBag.getNew();
    if (!textureHandle.valid())
    {
        DFLOG_WARN("Failed to create a 2d texture");
        return{};
    }

    auto maxSize = m_caps.maxTextureSize;
    if (width > maxSize || height > maxSize)
    {
        DFLOG_WARN("Failed to create a 2d texture: size is too big.");
        return{};
    }

    GLint pixelDataFormat = 0;
    GLint glInternalFormat = 0;
    switch (format)
    {
    case PixelFormat::RGB:
        pixelDataFormat = GL_RGB;
        glInternalFormat = GL_RGB;
        break;
    case PixelFormat::RGBA:
        pixelDataFormat = GL_RGBA;
        glInternalFormat = GL_RGBA;
        break;
    case PixelFormat::DEPTH:
        pixelDataFormat = GL_DEPTH_COMPONENT;
        glInternalFormat = GL_DEPTH_COMPONENT16;
        break;
    default:
        DFLOG_WARN("Invalid GL texture pixel format");
        return{};
    }

    TextureGL texture;

    GL_CHECK(glGenTextures(1, &texture.gl_id));
    if (texture.gl_id == 0)
        return{};

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture.gl_id));

    SetupGLWrapMode(GL_TEXTURE_2D, params.getWrapMode());
    SetupGLTextureFiltering(GL_TEXTURE_2D, params.getFiltering(), params.isMipmapped());

    // Init empty texture.
    if (format == PixelFormat::DEPTH)
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, pixelDataFormat, GL_FLOAT, nullptr));
    else
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, pixelDataFormat, GL_UNSIGNED_BYTE, nullptr));

    if (data)
        GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, pixelDataFormat, GL_UNSIGNED_BYTE, data));

    if (params.isMipmapped())
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

    if (m_anisotropicFilteringSupported && params.isAnisotropyMax())
    {
        if (m_caps.maxAnisotropy > 0.0f)
            GL_CHECK(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, std::max(1.0f, m_caps.maxAnisotropy)));
    }

    texture.type = GL_TEXTURE_2D;
    texture.pixelFormat = pixelDataFormat;

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

    m_textures[textureHandle.id] = texture;

    m_stats.textures++;

#ifdef _DEBUG
    m_gpuMemStats.addTexture(textureHandle, width * height * GetPixelSizeForFormat(format));    // TODO: mipmaps!
#endif

    return textureHandle;
}

TextureHandle RenderBackendGL::createTextureCube(unique_ptr<PixelBuffer> pixels[(size_t)CubeFace::COUNT], const TextureCreationParams &params)
{
    TextureHandle textureHandle = m_texturesBag.getNew();
    if (!textureHandle.valid())
    {
        DFLOG_WARN("Failed to create a cube texture");
        return{};
    }

    TextureGL texture;

    GL_CHECK(glGenTextures(1, &texture.gl_id));
    if (texture.gl_id == 0)
        return{};

    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, texture.gl_id));

    SetupGLWrapMode(GL_TEXTURE_CUBE_MAP, params.getWrapMode());
    SetupGLTextureFiltering(GL_TEXTURE_CUBE_MAP, params.getFiltering(), params.isMipmapped());

    size_t textureSizeInBytes = 0;

    for (int i = 0; i < (size_t)CubeFace::COUNT; i++)
    {
        GLint glPixelFormat = 0;
        switch (pixels[i]->getFormat())
        {
        case PixelFormat::RGB:
            glPixelFormat = GL_RGB;
            break;
        case PixelFormat::RGBA:
            glPixelFormat = GL_RGBA;
            break;
        default:
            DFLOG_WARN("Invalid GL texture pixel format");
            return{};
        }

        auto data = pixels[i]->getData();
        auto width = pixels[i]->getWidth();
        auto height = pixels[i]->getHeight();
        GL_CHECK(glTexImage2D(MapSidesToGL.find((CubeFace)i)->second, 0, glPixelFormat, width, height, 0, glPixelFormat, GL_UNSIGNED_BYTE, data));

        textureSizeInBytes += pixels[i]->getSizeInBytes();
    }

    if (params.isMipmapped())
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

    texture.type = GL_TEXTURE_CUBE_MAP;

    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));

    m_textures[textureHandle.id] = texture;

    m_stats.textures++;

#ifdef _DEBUG
    m_gpuMemStats.addTexture(textureHandle, textureSizeInBytes);    // TODO: mipmaps!
#endif

    return textureHandle;
}

void RenderBackendGL::updateTexture(TextureHandle textureHandle, int w, int h, const void *data)
{
    // FIXME: works only for 2d textures.
    HANDLE_CHECK(textureHandle);

    const auto &texture = m_textures[textureHandle.id];
    DF3D_ASSERT(texture.type != GL_INVALID_ENUM);

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture.gl_id));
    GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, texture.pixelFormat, GL_UNSIGNED_BYTE, data));
}

void RenderBackendGL::destroyTexture(TextureHandle textureHandle)
{
    HANDLE_CHECK(textureHandle);

    const auto &texture = m_textures[textureHandle.id];
    if (texture.type == GL_INVALID_ENUM)
    {
        DF3D_ASSERT(false);
        return;
    }

    GL_CHECK(glBindTexture(texture.type, 0));
    if (texture.gl_id)
        GL_CHECK(glDeleteTextures(1, &texture.gl_id));

    m_textures[textureHandle.id] = {};
    m_texturesBag.release(textureHandle.id);

    DF3D_ASSERT(m_stats.textures > 0);
    m_stats.textures--;

#ifdef _DEBUG
    m_gpuMemStats.removeTexture(textureHandle);
#endif
}

void RenderBackendGL::bindTexture(TextureHandle textureHandle, int unit)
{
    HANDLE_CHECK(textureHandle);

    const auto &texture = m_textures[textureHandle.id];
    DF3D_ASSERT(texture.gl_id != 0 && texture.type != GL_INVALID_ENUM);

    GL_CHECK(glActiveTexture(GL_TEXTURE0 + unit));
    GL_CHECK(glBindTexture(texture.type, texture.gl_id));
}

ShaderHandle RenderBackendGL::createShader(ShaderType type, const std::string &data)
{
    if (data.empty())
    {
        DFLOG_WARN("Failed to create a shader: empty shader data");
        return{};
    }

    ShaderHandle shaderHandle = m_shadersBag.getNew();
    if (!shaderHandle.valid())
    {
        DFLOG_WARN("Failed to create a shader");
        return{};
    }

    ShaderGL shader;

    if (type == ShaderType::VERTEX)
    {
        GL_CHECK(shader.gl_id = glCreateShader(GL_VERTEX_SHADER));
        shader.type = GL_VERTEX_SHADER;
    }
    else if (type == ShaderType::FRAGMENT)
    {
        GL_CHECK(shader.gl_id = glCreateShader(GL_FRAGMENT_SHADER));
        shader.type = GL_FRAGMENT_SHADER;
    }

    if (shader.gl_id == 0)
    {
        DFLOG_WARN("Failed to create a shader");
        return{};
    }

    // Compile the shader.
    const char *pdata = data.c_str();
    GL_CHECK(glShaderSource(shader.gl_id, 1, &pdata, nullptr));
    GL_CHECK(glCompileShader(shader.gl_id));

#ifdef _DEBUG
    int compileOk;
    GL_CHECK(glGetShaderiv(shader.gl_id, GL_COMPILE_STATUS, &compileOk));
    if (compileOk == GL_FALSE)
    {
        DFLOG_WARN("Failed to compile a shader");
        DFLOG_MESS("\n\n%s\n\n", data.c_str());
        PrintShaderLog(shader.gl_id);

        m_shadersBag.release(shaderHandle.id);
        GL_CHECK(glDeleteShader(shader.gl_id));

        DEBUG_BREAK();

        return{};
    }
#endif

    m_shaders[shaderHandle.id] = shader;

    return shaderHandle;
}

GpuProgramHandle RenderBackendGL::createGpuProgram(ShaderHandle vertexShaderHandle, ShaderHandle fragmentShaderHandle)
{
    HANDLE_CHECK_RETURN_INVALID(vertexShaderHandle);
    HANDLE_CHECK_RETURN_INVALID(fragmentShaderHandle);

    GpuProgramHandle programHandle = m_gpuProgramsBag.getNew();
    if (!programHandle.valid())
    {
        DFLOG_WARN("Failed to create a gpu program");
        return{};
    }

    ProgramGL program;

    GL_CHECK(program.gl_id = glCreateProgram());
    if (program.gl_id == 0)
    {
        DFLOG_WARN("Failed to create a GPU program");
        return{};
    }

    const auto &vertexShaderGL = m_shaders[vertexShaderHandle.id];
    const auto &fragmentShaderGL = m_shaders[fragmentShaderHandle.id];

    DF3D_ASSERT(vertexShaderGL.gl_id && fragmentShaderGL.gl_id);

    GL_CHECK(glAttachShader(program.gl_id, vertexShaderGL.gl_id));
    GL_CHECK(glAttachShader(program.gl_id, fragmentShaderGL.gl_id));

    // TODO: use VAO + refactor this.
    GL_CHECK(glBindAttribLocation(program.gl_id, VertexFormat::POSITION_3, "a_vertex3"));
    GL_CHECK(glBindAttribLocation(program.gl_id, VertexFormat::NORMAL_3, "a_normal"));
    GL_CHECK(glBindAttribLocation(program.gl_id, VertexFormat::TX_2, "a_txCoord"));
    GL_CHECK(glBindAttribLocation(program.gl_id, VertexFormat::COLOR_4, "a_vertexColor"));
    GL_CHECK(glBindAttribLocation(program.gl_id, VertexFormat::TANGENT_3, "a_tangent"));
    GL_CHECK(glBindAttribLocation(program.gl_id, VertexFormat::BITANGENT_3, "a_bitangent"));

    GL_CHECK(glLinkProgram(program.gl_id));

#ifdef _DEBUG
    int linkOk;
    GL_CHECK(glGetProgramiv(program.gl_id, GL_LINK_STATUS, &linkOk));
    if (linkOk == GL_FALSE)
    {
        DFLOG_WARN("GPU program linkage failed");
        PrintGpuProgramLog(program.gl_id);

        m_gpuProgramsBag.release(programHandle.id);
        GL_CHECK(glDeleteProgram(program.gl_id));

        return{};
    }
#endif
    program.vshader = vertexShaderHandle;
    program.fshader = fragmentShaderHandle;

    m_programs[programHandle.id] = program;

    return programHandle;
}

void RenderBackendGL::destroyGpuProgram(GpuProgramHandle programHandle)
{
    HANDLE_CHECK(programHandle);

    const auto &programGL = m_programs[programHandle.id];

    GL_CHECK(glUseProgram(0));
    if (programGL.gl_id != 0)
    {
        destroyShader(programGL.vshader, programGL.gl_id);
        destroyShader(programGL.fshader, programGL.gl_id);
        GL_CHECK(glDeleteProgram(programGL.gl_id));
    }
    else
        return;

    m_programs[programHandle.id] = {};

    m_gpuProgramsBag.release(programHandle.id);

    // Destroy associated uniforms
    auto programUniformsFound = m_programUniforms.find(programHandle.id);
    if (programUniformsFound != m_programUniforms.end())
    {
        for (auto uniHandle : programUniformsFound->second)
            m_uniformsBag.release(uniHandle.id);

        m_programUniforms.erase(programUniformsFound);
    }
}

void RenderBackendGL::bindGpuProgram(GpuProgramHandle programHandle)
{
    HANDLE_CHECK(programHandle);

    if (programHandle.id == m_currentProgram.id)
        return;

    const auto &programGL = m_programs[programHandle.id];
    DF3D_ASSERT(programGL.gl_id != 0);

    GL_CHECK(glUseProgram(programGL.gl_id));

    m_currentProgram = programHandle;
}

void RenderBackendGL::requestUniforms(GpuProgramHandle programHandle, std::vector<UniformHandle> &outHandles, std::vector<std::string> &outNames)
{
    HANDLE_CHECK(programHandle);

    const auto &programGL = m_programs[programHandle.id];

    DF3D_ASSERT(programGL.gl_id != 0);

    int total = -1;
    GL_CHECK(glGetProgramiv(programGL.gl_id, GL_ACTIVE_UNIFORMS, &total));

    for (int i = 0; i < total; i++)
    {
        UniformHandle uniformHandle = m_uniformsBag.getNew();
        if (!uniformHandle.valid())
        {
            DFLOG_WARN("Failed to request uniforms");
            outHandles.clear();
            return;
        }

        outHandles.push_back(uniformHandle);
    }

    m_programUniforms[programHandle.id].clear();
    auto programUniformsMap = m_programUniforms.find(programHandle.id);

    for (int i = 0; i < total; i++)
    {
        GLenum type = GL_INVALID_ENUM;
        int nameLength = -1, uniformVarSize = -1;
        char name[100];

        GL_CHECK(glGetActiveUniform(programGL.gl_id, i, sizeof(name) - 1, &nameLength, &uniformVarSize, &type, name));
        name[nameLength] = 0;

        outNames.push_back(name);

        UniformGL uniformGL;
        uniformGL.type = type;
        GL_CHECK(uniformGL.location = glGetUniformLocation(programGL.gl_id, name));

        m_uniforms[outHandles[i].id] = uniformGL;

        programUniformsMap->second.push_back(outHandles[i]);
    }
}

void RenderBackendGL::setUniformValue(UniformHandle uniformHandle, const void *data)
{
    HANDLE_CHECK(uniformHandle);

    const auto &uniformGL = m_uniforms[uniformHandle.id];

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

void RenderBackendGL::setViewport(int x, int y, int width, int height)
{
    GL_CHECK(glViewport(x, y, width, height));
}

void RenderBackendGL::clearColorBuffer(const glm::vec4 &color)
{
    GL_CHECK(glClearColor(color.r, color.g, color.b, color.a));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
}

void RenderBackendGL::clearDepthBuffer()
{
    GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT));
}

void RenderBackendGL::clearStencilBuffer()
{
    GL_CHECK(glClear(GL_STENCIL_BUFFER_BIT));
}

void RenderBackendGL::enableDepthTest(bool enable)
{
    if (m_drawState.depthTest == enable)
        return;

    if (enable)
        GL_CHECK(glEnable(GL_DEPTH_TEST));
    else
        GL_CHECK(glDisable(GL_DEPTH_TEST));

    m_drawState.depthTest = enable;
}

void RenderBackendGL::enableDepthWrite(bool enable)
{
    if (m_drawState.depthWrite == enable)
        return;

    GL_CHECK(glDepthMask(enable));

    m_drawState.depthWrite = enable;
}

void RenderBackendGL::enableScissorTest(bool enable)
{
    if (m_drawState.scissorTest == enable)
        return;

    if (enable)
        GL_CHECK(glEnable(GL_SCISSOR_TEST));
    else
        GL_CHECK(glDisable(GL_SCISSOR_TEST));

    m_drawState.scissorTest = enable;
}

void RenderBackendGL::setScissorRegion(int x, int y, int width, int height)
{
    GL_CHECK(glScissor(x, y, width, height));
}

void RenderBackendGL::setBlendingMode(BlendingMode mode)
{
    if (m_drawState.blendingMode == mode)
        return;

    bool enableBlending = true;

    switch (mode)
    {
    case BlendingMode::NONE:
        enableBlending = false;
        break;
    case BlendingMode::ADDALPHA:
        GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE));
        break;
    case BlendingMode::ALPHA:
        GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        break;
    case BlendingMode::ADD:
        GL_CHECK(glBlendFunc(GL_ONE, GL_ONE));
        break;
    default:
        break;
    }

    if (enableBlending != m_drawState.blendingEnabled)
    {
        if (enableBlending)
            GL_CHECK(glEnable(GL_BLEND));
        else
            GL_CHECK(glDisable(GL_BLEND));
        m_drawState.blendingEnabled = enableBlending;
    }

    m_drawState.blendingMode = mode;
}

void RenderBackendGL::setCullFaceMode(FaceCullMode mode)
{
    if (m_drawState.faceCullMode == mode)
        return;

    switch (mode)
    {
    case FaceCullMode::NONE:
        GL_CHECK(glDisable(GL_CULL_FACE));
        break;
    case FaceCullMode::FRONT:
        GL_CHECK(glEnable(GL_CULL_FACE));
        GL_CHECK(glCullFace(GL_FRONT));
        break;
    case FaceCullMode::BACK:
        GL_CHECK(glEnable(GL_CULL_FACE));
        GL_CHECK(glCullFace(GL_BACK));
        break;
    default:
        break;
    }

    m_drawState.faceCullMode = mode;
}

void RenderBackendGL::draw(Topology type, size_t numberOfElements)
{
    if (m_indexedDrawCall)
        GL_CHECK(glDrawElements(GetGLDrawMode(type), numberOfElements, m_currentIndexType, nullptr));
    else
        GL_CHECK(glDrawArrays(GetGLDrawMode(type), 0, numberOfElements));

    // Update stats.
#ifdef _DEBUG
    {
        m_stats.drawCalls++;
        switch (type)
        {
        case Topology::LINES:
            m_stats.totalLines += numberOfElements / 2;
            break;
        case Topology::TRIANGLES:
            m_stats.totalTriangles += numberOfElements / 3;
            break;
        case Topology::TRIANGLE_STRIP:
            if (numberOfElements >= 3)
                m_stats.totalTriangles += numberOfElements - 2;
            break;
        default:
            break;
        }
    }
#endif
}

unique_ptr<IRenderBackend> IRenderBackend::create(int width, int height)
{
    return make_unique<RenderBackendGL>(width, height);
}

}
