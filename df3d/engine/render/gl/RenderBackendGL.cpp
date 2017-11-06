#include "RenderBackendGL.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/render/RenderCommon.h>
#include <df3d/engine/render/RenderOperation.h>
#include <df3d/engine/resources/TextureResource.h>
#include <df3d/lib/Utils.h>

#ifndef DF3D_IOS
#error "implement"
#endif

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

static size_t GetTextureSize(GLint glFormat, size_t w, size_t h, bool mipmaps)
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

    return static_cast<size_t>(result);
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

static bool IsDepthTexture(PixelFormat fmt)
{
    return fmt == PixelFormat::DEPTH;
}

static const std::unordered_map<CubeFace, GLenum> MapSidesToGL =
{
    { CubeFace::POSITIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_X },
    { CubeFace::NEGATIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X },
    { CubeFace::POSITIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Y },
    { CubeFace::NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y },
    { CubeFace::POSITIVE_Z, GL_TEXTURE_CUBE_MAP_POSITIVE_Z },
    { CubeFace::NEGATIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z }
};

static GLint GetGLWrapMode(uint32_t flags)
{
    const auto mode = flags & TEXTURE_WRAP_MODE_MASK;

    if (mode == TEXTURE_WRAP_MODE_CLAMP)
        return GL_CLAMP_TO_EDGE;
    else if (mode == TEXTURE_WRAP_MODE_REPEAT)
        return GL_REPEAT;

    DFLOG_WARN("GetGLWrapMode was set to default: GL_REPEAT");

    return GL_REPEAT;
}

static void SetupGLTextureFiltering(GLenum glType, uint32_t flags)
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

static void SetupGLWrapMode(GLenum glType, uint32_t flags)
{
    auto wmGL = GetGLWrapMode(flags);

    GL_CHECK(glTexParameteri(glType, GL_TEXTURE_WRAP_S, wmGL));
    GL_CHECK(glTexParameteri(glType, GL_TEXTURE_WRAP_T, wmGL));
#if defined(DF3D_DESKTOP)
    GL_CHECK(glTexParameteri(glType, GL_TEXTURE_WRAP_R, wmGL));
#endif
}

static GLenum GetGLBufferUsageType(GpuBufferUsageType usageType)
{
    switch (usageType)
    {
    case GpuBufferUsageType::STATIC:
        return GL_STATIC_DRAW;
    case GpuBufferUsageType::DYNAMIC:
        return GL_STREAM_DRAW;
    default:
        break;
    }

    return GL_INVALID_ENUM;
}

static GLenum GetGLDrawMode(Topology topologyType)
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
static void PrintShaderLog(GLuint shader)
{
    int infologLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);

    unique_ptr<char[]> infoLog(new char[infologLen + 1]);
    glGetShaderInfoLog(shader, infologLen, nullptr, infoLog.get());

    DFLOG_MESS("Shader info log: %s", infoLog.get());
    DF3D_ASSERT(false);
}

static void PrintGpuProgramLog(unsigned int program)
{
    int infologLen = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLen);

    unique_ptr<char[]> infoLog(new char[infologLen + 1]);
    glGetProgramInfoLog(program, infologLen, nullptr, infoLog.get());

    DFLOG_MESS("GPU program info log: %s", infoLog.get());
    DF3D_ASSERT(false);
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
    DF3D_ASSERT(m_shadersBag.isValid(shader.getID()));

    const auto &shaderGL = m_shaders[shader.getIndex()];
    DF3D_ASSERT(shaderGL.glID != 0);

    if (!m_destroyAndroidWorkaround) {
        GL_CHECK(glDetachShader(programId, shaderGL.glID));
        GL_CHECK(glDeleteShader(shaderGL.glID));
    }

    m_shaders[shader.getIndex()] = {};

    m_shadersBag.release(shader.getID());
}

RenderBackendGL::RenderBackendGL(int width, int height)
    : m_vertexBuffersBag(MemoryManager::allocDefault()),
    m_indexBuffersBag(MemoryManager::allocDefault()),
    m_texturesBag(MemoryManager::allocDefault()),
    m_shadersBag(MemoryManager::allocDefault()),
    m_gpuProgramsBag(MemoryManager::allocDefault()),
    m_uniformsBag(MemoryManager::allocDefault()),
    m_framebuffersBag(MemoryManager::allocDefault())
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

    initExtensions();

    std::fill(std::begin(m_vertexBuffers), std::end(m_vertexBuffers), VertexBufferGL());
    std::fill(std::begin(m_indexBuffers), std::end(m_indexBuffers), IndexBufferGL());
    std::fill(std::begin(m_textures), std::end(m_textures), TextureGL());
    std::fill(std::begin(m_shaders), std::end(m_shaders), ShaderGL());
    std::fill(std::begin(m_programs), std::end(m_programs), ProgramGL());
    std::fill(std::begin(m_uniforms), std::end(m_uniforms), UniformGL());
    std::fill(std::begin(m_frameBuffers), std::end(m_frameBuffers), FrameBufferGL());

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

    // WORKAROUND
    if (m_caps.maxTextureSize < 2048)
        throw std::runtime_error("Hardware not supported");

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
        sizeof(m_frameBuffers) +
        sizeof(m_uniforms);

    DFLOG_DEBUG("RenderBackendGL storage %d KB", utils::sizeKB(totalStorage));
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
    DF3D_ASSERT(m_shadersBag.empty());
    DF3D_ASSERT(m_gpuProgramsBag.empty());
    DF3D_ASSERT(m_uniformsBag.empty());
    DF3D_ASSERT(m_framebuffersBag.empty());
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

void RenderBackendGL::frameBegin()
{
    m_stats.drawCalls = m_stats.totalLines = m_stats.totalTriangles = 0;

    m_indexedDrawCall = false;
    m_currentIndexType = GL_INVALID_ENUM;
    m_currentProgram = {};
    m_currentVertexBuffer = {};
    m_currentIndexBuffer = {};

    GL_CHECK(glUseProgram(0));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

void RenderBackendGL::frameEnd()
{

}

VertexBufferHandle RenderBackendGL::createVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data)
{
    DF3D_ASSERT(verticesCount > 0);

    VertexBufferGL vertexBuffer;

    vertexBuffer.format = format;
    vertexBuffer.sizeInBytes = verticesCount * format.getVertexSize();

    GL_CHECK(glGenBuffers(1, &vertexBuffer.glID));
    if (vertexBuffer.glID == 0)
    {
        DFLOG_WARN("glGenBuffers failed for RenderBackendGL::createVertexBuffer");
        return{};
    }

    auto vbHandle = VertexBufferHandle(m_vertexBuffersBag.getNew());

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.glID));
    //////
    //////
    //////
    //////
    //////
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertexBuffer.sizeInBytes, data, GL_DYNAMIC_DRAW));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));

    m_vertexBuffers[vbHandle.getIndex()] = vertexBuffer;

    m_currentVertexBuffer = {};

#ifdef _DEBUG
    m_gpuMemStats.addVertexBuffer(vbHandle, vertexBuffer.sizeInBytes);
#endif

    return vbHandle;
}

VertexBufferHandle RenderBackendGL::createDynamicVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data)
{
    ////////////////////////
    ////////////////////////
    ////////////////////////
    ////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////
    ////////////////////////
    ////////////////////////
    ////////////////////////
    return createVertexBuffer(format, verticesCount, data);
}

void RenderBackendGL::destroyVertexBuffer(VertexBufferHandle vbHandle)
{
    DF3D_ASSERT(m_vertexBuffersBag.isValid(vbHandle.getID()));

    const auto &vertexBuffer = m_vertexBuffers[vbHandle.getIndex()];

    if (!m_destroyAndroidWorkaround) {
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GL_CHECK(glDeleteBuffers(1, &vertexBuffer.glID));
    }

    m_vertexBuffers[vbHandle.getIndex()] = {};
    m_vertexBuffersBag.release(vbHandle.getID());
    m_currentVertexBuffer = {};

#ifdef _DEBUG
    m_gpuMemStats.removeVertexBuffer(vbHandle);
#endif
}

void RenderBackendGL::bindVertexBuffer(VertexBufferHandle vbHandle, size_t vertexBufferOffset)
{
    DF3D_ASSERT(m_vertexBuffersBag.isValid(vbHandle.getID()));

    // FIXME:
    //if (m_currentVertexBuffer.valid() && vb.id == m_currentVertexBuffer.id)
    //    return;

    const auto &vertexBuffer = m_vertexBuffers[vbHandle.getIndex()];

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.glID));

    const auto &format = vertexBuffer.format;
    const auto vertexSize = format.getVertexSize();

    for (uint16_t i = VertexFormat::POSITION; i != VertexFormat::COUNT; i++)
    {
        auto attrib = (VertexFormat::VertexAttribute)i;

        if (!format.hasAttribute(attrib))
            continue;

        GL_CHECK(glEnableVertexAttribArray(attrib));
        size_t offset = format.getOffsetTo(attrib) + vertexBufferOffset * format.getVertexSize();
        size_t count = format.getCompCount(attrib);

        GL_CHECK(glVertexAttribPointer(attrib, count, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*)offset));
    }

    m_indexedDrawCall = false;
    m_currentVertexBuffer = vbHandle;
}

void RenderBackendGL::updateDynamicVertexBuffer(VertexBufferHandle vbHandle, size_t verticesCount, const void *data)
{
        ///////
    ///////
    ///////
    ///////
    ///////
    ///////
    ///////
    ///////
    ///////
    ///////



    DF3D_ASSERT(m_vertexBuffersBag.isValid(vbHandle.getID()));

    const auto &vertexBuffer = m_vertexBuffers[vbHandle.getIndex()];

    auto bytesUpdating = verticesCount * vertexBuffer.format.getVertexSize();
    DF3D_ASSERT(bytesUpdating <= vertexBuffer.sizeInBytes);

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.glID));
    GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, bytesUpdating, data));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));

    m_currentVertexBuffer = {};
}

IndexBufferHandle RenderBackendGL::createIndexBuffer(size_t indicesCount, const void *data, IndicesType indicesType)
{
    // NOTE: some GPUs do not support 32-bit indices (Mali 400)

    DF3D_ASSERT(indicesCount > 0);

    IndexBufferGL indexBuffer;

    GL_CHECK(glGenBuffers(1, &indexBuffer.glID));
    if (indexBuffer.glID == 0)
    {
        DFLOG_WARN("glGenBuffers failed for RenderBackendGL::createIndexBuffer");
        return{};
    }

    auto ibHandle = IndexBufferHandle(m_indexBuffersBag.getNew());

    indexBuffer.indices16bit = (indicesType == INDICES_16_BIT);
    indexBuffer.sizeInBytes = indicesCount * (indexBuffer.indices16bit ? sizeof(uint16_t) : sizeof(uint32_t));

    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.glID));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.sizeInBytes, data, GL_STATIC_DRAW));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    m_indexBuffers[ibHandle.getIndex()] = indexBuffer;
    m_currentIndexBuffer = {};

#ifdef _DEBUG
    m_gpuMemStats.addIndexBuffer(ibHandle, indexBuffer.sizeInBytes);
#endif

    return ibHandle;
}

void RenderBackendGL::destroyIndexBuffer(IndexBufferHandle ibHandle)
{
    DF3D_ASSERT(m_indexBuffersBag.isValid(ibHandle.getID()));

    const auto &indexBuffer = m_indexBuffers[ibHandle.getIndex()];

    if (!m_destroyAndroidWorkaround) {
        GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        GL_CHECK(glDeleteBuffers(1, &indexBuffer.glID));
    }

    m_indexBuffers[ibHandle.getIndex()] = {};
    m_indexBuffersBag.release(ibHandle.getID());
    m_currentIndexBuffer = {};

#ifdef _DEBUG
    m_gpuMemStats.removeIndexBuffer(ibHandle);
#endif
}

void RenderBackendGL::bindIndexBuffer(IndexBufferHandle ibHandle)
{
    DF3D_ASSERT(m_indexBuffersBag.isValid(ibHandle.getID()));

    //if (m_currentIndexBuffer.valid() && ib.id == m_currentIndexBuffer.id)
    //    return;

    const auto &indexBuffer = m_indexBuffers[ibHandle.getIndex()];
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.glID));

    m_indexedDrawCall = true;
    m_currentIndexType = indexBuffer.indices16bit ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
    m_currentIndexBuffer = ibHandle;
}

// void RenderBackendGL::updateIndexBuffer(IndexBufferHandle ibHandle, size_t indicesCount, const void *data)
// {
//     DF3D_ASSERT(m_indexBuffersBag.isValid(ibHandle.getID()));

//     const auto &indexBuffer = m_indexBuffers[ibHandle.getIndex()];

//     auto bytesUpdating = indicesCount * (indexBuffer.indices16bit ? sizeof(uint16_t) : sizeof(uint32_t));
//     DF3D_ASSERT(bytesUpdating <= indexBuffer.sizeInBytes);

//     GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.glID));
//     GL_CHECK(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bytesUpdating, data));
//     GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

//     m_currentIndexBuffer = {};
// }

TextureHandle RenderBackendGL::createTexture(const TextureResourceData &data, uint32_t flags)
{
    DF3D_ASSERT(data.mipLevels.size() > 0);

    int width = data.mipLevels[0].width;
    int height = data.mipLevels[0].height;

    size_t maxSize = m_caps.maxTextureSize;
    if (width > maxSize || height > maxSize)
    {
        DFLOG_WARN("Failed to create a 2D texture: size is too big. Max size: %d", maxSize);
        return{};
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
        return {};
    }

    TextureGL texture;

    GL_CHECK(glGenTextures(1, &texture.glID));
    if (texture.glID == 0)
    {
        DFLOG_WARN("glGenTextures failed for RenderBackendGL::createTexture2D");
        return {};
    }

    GLint previousUnpackAlignment;
    if (data.format == PixelFormat::KTX)
    {
        // KTX files require an unpack alignment of 4
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousUnpackAlignment);
        if (previousUnpackAlignment != 4)
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }

    auto textureHandle = TextureHandle(m_texturesBag.getNew());

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture.glID));

    SetupGLWrapMode(GL_TEXTURE_2D, flags);
    SetupGLTextureFiltering(GL_TEXTURE_2D, flags);

    size_t debugTotalSize = 0;

    if (data.format == PixelFormat::KTX)
    {
        for (size_t mip = 0; mip < data.mipLevels.size(); mip++)
        {
            const auto &mipLevel = data.mipLevels[mip];

            GL_CHECK(glCompressedTexImage2D(GL_TEXTURE_2D, mip, glInternalFormat, mipLevel.width, mipLevel.height, 0, mipLevel.pixels.size(), mipLevel.pixels.data()));

            debugTotalSize += mipLevel.pixels.size();
        }
    }
    else
    {
        DF3D_ASSERT(data.mipLevels.size() == 1);
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, pixelDataFormat, GL_UNSIGNED_BYTE, data.mipLevels[0].pixels.data()));

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

        debugTotalSize = GetTextureSize(glInternalFormat, width, height, mipmapped);
    }

    if (m_anisotropicFilteringSupported && m_caps.maxAnisotropy > 0.0f)
    {
        if ((flags & TEXTURE_FILTERING_MASK) == TEXTURE_FILTERING_ANISOTROPIC)
            GL_CHECK(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, std::max(1.0f, m_caps.maxAnisotropy)));
    }

    texture.type = GL_TEXTURE_2D;
    texture.pixelFormat = pixelDataFormat;

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

    m_textures[textureHandle.getIndex()] = texture;

    m_stats.textures++;

#ifdef _DEBUG
    m_gpuMemStats.addTexture(textureHandle, debugTotalSize);
#endif

    if (data.format == PixelFormat::KTX)
    {
        if (previousUnpackAlignment != 4)
            glPixelStorei(GL_UNPACK_ALIGNMENT, previousUnpackAlignment);
    }

    return textureHandle;
}

void RenderBackendGL::updateTexture(TextureHandle textureHandle, int w, int h, const void *data)
{
    // FIXME: works only for 2D textures.
    DF3D_ASSERT(m_texturesBag.isValid(textureHandle.getID()));

    const auto &texture = m_textures[textureHandle.getIndex()];
    DF3D_ASSERT(texture.type != GL_INVALID_ENUM);
    if (texture.pixelFormat == GL_INVALID_ENUM)
    {
        DF3D_ASSERT(false);
        return;
    }

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture.glID));
    GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, texture.pixelFormat, GL_UNSIGNED_BYTE, data));
}

void RenderBackendGL::destroyTexture(TextureHandle textureHandle)
{
    DF3D_ASSERT(m_texturesBag.isValid(textureHandle.getID()));

    const auto &texture = m_textures[textureHandle.getIndex()];
    if (texture.type == GL_INVALID_ENUM)
    {
        DF3D_ASSERT(false);
        return;
    }

    if (!m_destroyAndroidWorkaround) {
        GL_CHECK(glBindTexture(texture.type, 0));
    }
    if (texture.glID) {
        if (!m_destroyAndroidWorkaround) {
            GL_CHECK(glDeleteTextures(1, &texture.glID));
        }
    }

    m_textures[textureHandle.getIndex()] = {};
    m_texturesBag.release(textureHandle.getID());

    DF3D_ASSERT(m_stats.textures > 0);
    m_stats.textures--;

#ifdef _DEBUG
    m_gpuMemStats.removeTexture(textureHandle);
#endif
}

void RenderBackendGL::bindTexture(TextureHandle textureHandle, int unit)
{
    DF3D_ASSERT(m_texturesBag.isValid(textureHandle.getID()));

    const auto &texture = m_textures[textureHandle.getIndex()];

    GL_CHECK(glActiveTexture(GL_TEXTURE0 + unit));
    GL_CHECK(glBindTexture(texture.type, texture.glID));
}

ShaderHandle RenderBackendGL::createShader(ShaderType type, const char *data)
{
    if (!data)
    {
        DFLOG_WARN("Failed to create a shader: empty shader data");
        return{};
    }

    ShaderGL shader;

    if (type == ShaderType::VERTEX)
    {
        GL_CHECK(shader.glID = glCreateShader(GL_VERTEX_SHADER));
        shader.type = GL_VERTEX_SHADER;
    }
    else if (type == ShaderType::FRAGMENT)
    {
        GL_CHECK(shader.glID = glCreateShader(GL_FRAGMENT_SHADER));
        shader.type = GL_FRAGMENT_SHADER;
    }

    if (shader.glID == 0)
    {
        DFLOG_WARN("Failed to create a shader");
        return{};
    }

    // Compile the shader.
    GL_CHECK(glShaderSource(shader.glID, 1, &data, nullptr));
    GL_CHECK(glCompileShader(shader.glID));

#ifdef _DEBUG
    int compileOk;
    GL_CHECK(glGetShaderiv(shader.glID, GL_COMPILE_STATUS, &compileOk));
    if (compileOk == GL_FALSE)
    {
        DFLOG_WARN("Failed to compile a shader");
        DFLOG_MESS("\n\n%s\n\n", data);
        PrintShaderLog(shader.glID);

        GL_CHECK(glDeleteShader(shader.glID));

        DEBUG_BREAK();

        return{};
    }
#endif

    auto shaderHandle = ShaderHandle(m_shadersBag.getNew());
    m_shaders[shaderHandle.getIndex()] = shader;

    return shaderHandle;
}

GpuProgramHandle RenderBackendGL::createGpuProgram(ShaderHandle vertexShaderHandle, ShaderHandle fragmentShaderHandle)
{
    DF3D_ASSERT(m_shadersBag.isValid(vertexShaderHandle.getID()));
    DF3D_ASSERT(m_shadersBag.isValid(fragmentShaderHandle.getID()));

    ProgramGL program;

    GL_CHECK(program.glID = glCreateProgram());
    if (program.glID == 0)
    {
        DFLOG_WARN("Failed to create a GPU program");
        return{};
    }

    const auto &vertexShaderGL = m_shaders[vertexShaderHandle.getIndex()];
    const auto &fragmentShaderGL = m_shaders[fragmentShaderHandle.getIndex()];

    GL_CHECK(glAttachShader(program.glID, vertexShaderGL.glID));
    GL_CHECK(glAttachShader(program.glID, fragmentShaderGL.glID));

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
        PrintGpuProgramLog(program.glID);

        GL_CHECK(glDeleteProgram(program.glID));

        return{};
    }
#endif
    program.vshader = vertexShaderHandle;
    program.fshader = fragmentShaderHandle;

    auto programHandle = GpuProgramHandle(m_gpuProgramsBag.getNew());
    m_programs[programHandle.getIndex()] = program;

    return programHandle;
}

void RenderBackendGL::destroyGpuProgram(GpuProgramHandle programHandle)
{
    DF3D_ASSERT(m_gpuProgramsBag.isValid(programHandle.getID()));

    const auto &programGL = m_programs[programHandle.getIndex()];

    if (!m_destroyAndroidWorkaround) {
        GL_CHECK(glUseProgram(0));
    }
    destroyShader(programGL.vshader, programGL.glID);
    destroyShader(programGL.fshader, programGL.glID);
    if (!m_destroyAndroidWorkaround) {
        GL_CHECK(glDeleteProgram(programGL.glID));
    }

    m_programs[programHandle.getIndex()] = {};
    m_gpuProgramsBag.release(programHandle.getID());

    // Destroy associated uniforms
    auto programUniformsFound = m_programUniforms.find(programHandle);
    if (programUniformsFound != m_programUniforms.end())
    {
        for (auto uniHandle : programUniformsFound->second)
            m_uniformsBag.release(uniHandle.getID());

        m_programUniforms.erase(programUniformsFound);
    }
}

FrameBufferHandle RenderBackendGL::createFrameBuffer(TextureHandle *attachments, size_t attachmentCount)
{
    DF3D_ASSERT_MESS(false, "Not implemented");

    /*

    auto fbHandle = FrameBufferHandle(m_framebuffersBag.getNew());

    FrameBufferGL framebufferGL;
    GL_CHECK(glGenFramebuffers(1, &framebufferGL.fbo));
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, framebufferGL.fbo));

    for (size_t i = 0; i < attachmentCount; i++)
    {
        // TODO: render targets!
        const auto &textureGL = m_textures[attachments[i].getIndex()];
        if (IsDepthTexture(textureGL.info.format))
            GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureGL.glID, 0));
        else
            GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureGL.glID, 0));
    }

    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    m_frameBuffers[fbHandle.getIndex()] = framebufferGL;

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        DFLOG_WARN("Failed to create GL framebuffer!");

    return fbHandle;
     */
    return {};
}

void RenderBackendGL::destroyFrameBuffer(FrameBufferHandle framebufferHandle)
{
    DF3D_ASSERT(m_framebuffersBag.isValid(framebufferHandle.getID()));

    const auto &framebufferGL = m_frameBuffers[framebufferHandle.getIndex()];
    if (!m_destroyAndroidWorkaround) {
        GL_CHECK(glDeleteFramebuffers(1, &framebufferGL.fbo));
    }

    m_frameBuffers[framebufferHandle.getIndex()] = {};
    m_framebuffersBag.release(framebufferHandle.getID());
}

void RenderBackendGL::bindGpuProgram(GpuProgramHandle programHandle)
{
    DF3D_ASSERT(m_gpuProgramsBag.isValid(programHandle.getID()));

    if (programHandle == m_currentProgram)
        return;

    const auto &programGL = m_programs[programHandle.getIndex()];
    DF3D_ASSERT(programGL.glID != 0);

    GL_CHECK(glUseProgram(programGL.glID));

    m_currentProgram = programHandle;
}

void RenderBackendGL::requestUniforms(GpuProgramHandle programHandle, std::vector<UniformHandle> &outHandles, std::vector<std::string> &outNames)
{
    DF3D_ASSERT(m_gpuProgramsBag.isValid(programHandle.getID()));

    const auto &programGL = m_programs[programHandle.getIndex()];

    DF3D_ASSERT(programGL.glID != 0);

    int total = 0;
    GL_CHECK(glGetProgramiv(programGL.glID, GL_ACTIVE_UNIFORMS, &total));

    for (int i = 0; i < total; i++)
    {
        auto uniformHandle = UniformHandle(m_uniformsBag.getNew());

        outHandles.push_back(uniformHandle);
    }

    m_programUniforms[programHandle].clear();
    auto programUniformsMap = m_programUniforms.find(programHandle);

    for (int i = 0; i < total; i++)
    {
        GLenum type = GL_INVALID_ENUM;
        int nameLength = -1, uniformVarSize = -1;
        char name[256];

        GL_CHECK(glGetActiveUniform(programGL.glID, i, sizeof(name) - 1, &nameLength, &uniformVarSize, &type, name));
        name[nameLength] = 0;

        outNames.push_back(name);

        UniformGL uniformGL;
        uniformGL.type = type;
        GL_CHECK(uniformGL.location = glGetUniformLocation(programGL.glID, name));

        m_uniforms[outHandles[i].getIndex()] = uniformGL;

        programUniformsMap->second.push_back(outHandles[i]);
    }
}

void RenderBackendGL::setUniformValue(GpuProgramHandle programHandle, UniformHandle uniformHandle, const void *data)
{
    DF3D_ASSERT(m_uniformsBag.isValid(uniformHandle.getID()));

    const auto &uniformGL = m_uniforms[uniformHandle.getIndex()];

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

void RenderBackendGL::bindFrameBuffer(FrameBufferHandle frameBufferHandle)
{
    if (!frameBufferHandle.isValid())
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    else
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffers[frameBufferHandle.getIndex()].fbo);
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

}
