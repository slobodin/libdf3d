#include "RenderBackendGL.h"

#include <libdf3d/render/RenderCommon.h>
#include <libdf3d/render/Texture.h>
#include <libdf3d/render/RenderOperation.h>
#include <libdf3d/render/GpuProgram.h>
#include <libdf3d/utils/Utils.h>

namespace df3d {

static std::string CheckGLError()
{
    GLenum errCode = glGetError();
    if (errCode == GL_NO_ERROR)
        return "";

#if defined(DF3D_DESKTOP)
    std::string errString = (char *)gluErrorString(errCode);
    return errString;
#else
    switch (errCode)
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

static void CheckAndPrintGLError(const char *file, int line)
{
    auto err = CheckGLError();
    if (!err.empty())
        DFLOG_WARN("OpenGL error: %s. File: %s, line: %d", err.c_str(), file, line);
}

#if defined(_DEBUG) || defined(DEBUG)
#define printOpenGLError() CheckAndPrintGLError(__FILE__, __LINE__)
#else
#define printOpenGLError()
#endif

#if defined(_DEBUG) || defined(DEBUG)
#define DESCRIPTOR_CHECK(descr) do { if (!descr.valid()) { DFLOG_WARN("Invalid descriptor"); DEBUG_BREAK(); return; } } while (0);
#else
#define DESCRIPTOR_CHECK(descr) do { if (!descr.valid()) return; } while (0);
#endif

#if defined(_DEBUG) || defined(DEBUG)
#define DESCRIPTOR_CHECK_RETURN_INVALID(descr) do { if (!descr.valid()) { DFLOG_WARN("Invalid descriptor"); return {}; } } while (0);
#else
#define DESCRIPTOR_CHECK_RETURN_INVALID(descr) do { if (!descr.valid()) return {}; } while (0);
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
    glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, filtering == TextureFiltering::NEAREST ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GetGLFilteringMode(filtering, mipmapped));

    printOpenGLError();
}

static void SetupGLWrapMode(GLenum glType, TextureWrapMode wrapMode)
{
    auto wmGl = GetGLWrapMode(wrapMode);
    glTexParameteri(glType, GL_TEXTURE_WRAP_S, wmGl);
    glTexParameteri(glType, GL_TEXTURE_WRAP_T, wmGl);
#if defined(DF3D_DESKTOP)
    glTexParameteri(glType, GL_TEXTURE_WRAP_R, wmGl);
#endif

    printOpenGLError();
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

static GLenum GetGLDrawMode(RopType type)
{
    switch (type)
    {
    case RopType::LINES:
        return GL_LINES;
    case RopType::TRIANGLES:
        return GL_TRIANGLES;
    case RopType::LINE_STRIP:
        return GL_LINE_STRIP;
    case RopType::TRIANGLE_STRIP:
        return GL_TRIANGLE_STRIP;
    default:
        break;
    }

    return GL_INVALID_ENUM;
}

static bool IsSampler(GLenum type)
{
#if defined(DF3D_DESKTOP)
    return type == GL_SAMPLER_1D || type == GL_SAMPLER_2D || type == GL_SAMPLER_3D || type == GL_SAMPLER_CUBE;
#else
    return type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE;
#endif
}

static void PrintShaderLog(GLuint shader)
{
    int infologLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);

    unique_ptr<char> infoLog(new char[infologLen + 1]);
    glGetShaderInfoLog(shader, infologLen, nullptr, infoLog.get());

    DFLOG_MESS("Shader info log: %s", infoLog.get());
}

static void PrintGpuProgramLog(unsigned int program)
{
    int infologLen = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLen);

    unique_ptr<char> infoLog(new char[infologLen + 1]);
    glGetProgramInfoLog(program, infologLen, nullptr, infoLog.get());

    DFLOG_MESS("GPU program info log: %s", infoLog.get());
}

RenderBackendGL::RenderBackendGL()
    : m_vertexBuffersBag(MAX_SIZE),
    m_indexBuffersBag(MAX_SIZE),
    m_texturesBag(MAX_SIZE),
    m_shadersBag(MAX_SIZE),
    m_gpuProgramsBag(MAX_SIZE),
    m_uniformsBag(MAX_SIZE)
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
    return m_stats;
}

void RenderBackendGL::initialize()
{
    m_vertexBuffersBag = { MAX_SIZE };
    m_indexBuffersBag = { MAX_SIZE };
    m_texturesBag = { MAX_SIZE };
    m_shadersBag = { MAX_SIZE };
    m_gpuProgramsBag = { MAX_SIZE };
    m_uniformsBag = { MAX_SIZE };

    m_programUniforms.clear();

    std::fill(std::begin(m_vertexBuffers), std::end(m_vertexBuffers), VertexBufferGL());
    std::fill(std::begin(m_indexBuffers), std::end(m_indexBuffers), IndexBufferGL());
    std::fill(std::begin(m_textures), std::end(m_textures), TextureGL());
    std::fill(std::begin(m_shaders), std::end(m_shaders), ShaderGL());
    std::fill(std::begin(m_programs), std::end(m_programs), ProgramGL());
    std::fill(std::begin(m_uniforms), std::end(m_uniforms), UniformGL());

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glFrontFace(GL_CCW);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_caps.maxTextureSize);
    // TODO:
    // Check extension supported.
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &m_caps.maxAnisotropy);

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

    printOpenGLError();

#ifdef _DEBUG
    size_t totalStorage = sizeof(m_vertexBuffers) +
        sizeof(m_indexBuffers) +
        sizeof(m_textures) +
        sizeof(m_shaders) +
        sizeof(m_programs) +
        sizeof(m_uniforms);

    DFLOG_DEBUG("RenderBackendGL storage %d KB", utils::sizeKB(totalStorage));
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
    m_currentProgram = {};
    m_currentVertexBuffer = {};
    m_currentIndexBuffer = {};

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // Clear previous frame GL error.
    printOpenGLError();
}

void RenderBackendGL::frameEnd()
{
    glFlush();
}

df3d::VertexBufferDescriptor RenderBackendGL::createVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data, GpuBufferUsageType usage)
{
    DF3D_ASSERT(verticesCount > 0, "sanity check");

    VertexBufferDescriptor vb = { m_vertexBuffersBag.getNew() };
    if (!vb.valid())
    {
        DFLOG_WARN("Failed to create a vertex buffer");
        return{};
    }

    VertexBufferGL vertexBuffer;

    vertexBuffer.format = make_shared<VertexFormat>(format);
    vertexBuffer.sizeInBytes = verticesCount * format.getVertexSize();

    glGenBuffers(1, &vertexBuffer.gl_id);
    if (vertexBuffer.gl_id == 0)
        return{};

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.gl_id);
    glBufferData(GL_ARRAY_BUFFER, verticesCount * format.getVertexSize(), data, GetGLBufferUsageType(usage));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_vertexBuffers[vb.id] = std::move(vertexBuffer);

    m_currentVertexBuffer = {};

    return vb;
}

void RenderBackendGL::destroyVertexBuffer(VertexBufferDescriptor vb)
{
    DESCRIPTOR_CHECK(vb);

    const auto &vertexBuffer = m_vertexBuffers[vb.id];

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (vertexBuffer.gl_id)
        glDeleteBuffers(1, &vertexBuffer.gl_id);
    else
        return;

    m_vertexBuffers[vb.id] = {};

    m_vertexBuffersBag.release(vb.id);

    m_currentVertexBuffer = {};
}

void RenderBackendGL::bindVertexBuffer(VertexBufferDescriptor vb)
{
    DESCRIPTOR_CHECK(vb);

    // FIXME:
    //if (m_currentVertexBuffer.valid() && vb.id == m_currentVertexBuffer.id)
    //    return;

    const auto &vertexBuffer = m_vertexBuffers[vb.id];
    DF3D_ASSERT(vertexBuffer.gl_id != 0, "sanity check");

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.gl_id);

    const auto &format = *vertexBuffer.format;

    for (auto attrib : format.m_attribs)
    {
        glEnableVertexAttribArray(attrib);
        size_t offs = format.getOffsetTo(attrib);
        glVertexAttribPointer(attrib, format.m_counts[attrib], GL_FLOAT, GL_FALSE, format.getVertexSize(), (const GLvoid*)offs);
    }

    m_indexedDrawCall = false;
    m_currentVertexBuffer = vb;
}

void RenderBackendGL::updateVertexBuffer(VertexBufferDescriptor vb, size_t verticesCount, const void *data)
{
    DESCRIPTOR_CHECK(vb);

    const auto &vertexBuffer = m_vertexBuffers[vb.id];
    DF3D_ASSERT(vertexBuffer.gl_id != 0, "sanity check");

    auto bytesUpdating = verticesCount * vertexBuffer.format->getVertexSize();

    DF3D_ASSERT(bytesUpdating <= vertexBuffer.sizeInBytes, "sanity check");
    DF3D_ASSERT(vertexBuffer.gl_id != 0, "sanity check");

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.gl_id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, bytesUpdating, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_currentVertexBuffer = {};
}

df3d::IndexBufferDescriptor RenderBackendGL::createIndexBuffer(size_t indicesCount, const void *data, GpuBufferUsageType usage)
{
    DF3D_ASSERT(indicesCount > 0, "sanity check");

    IndexBufferDescriptor ib = { m_indexBuffersBag.getNew() };
    if (!ib.valid())
    {
        DFLOG_WARN("Failed to create an index buffer");
        return{};
    }

    IndexBufferGL indexBuffer;

    glGenBuffers(1, &indexBuffer.gl_id);
    if (indexBuffer.gl_id == 0)
        return {};

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.gl_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(INDICES_TYPE), data, GetGLBufferUsageType(usage));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    indexBuffer.sizeInBytes = indicesCount * sizeof(INDICES_TYPE);

    m_indexBuffers[ib.id] = indexBuffer;
    m_currentIndexBuffer = {};

    return ib;
}

void RenderBackendGL::destroyIndexBuffer(IndexBufferDescriptor ib)
{
    DESCRIPTOR_CHECK(ib);

    const auto &indexBuffer = m_indexBuffers[ib.id];

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    if (indexBuffer.gl_id != 0)
        glDeleteBuffers(1, &indexBuffer.gl_id);
    else
        return;

    m_indexBuffers[ib.id] = {};

    m_indexBuffersBag.release(ib.id);

    m_currentIndexBuffer = {};
}

void RenderBackendGL::bindIndexBuffer(IndexBufferDescriptor ib)
{
    DESCRIPTOR_CHECK(ib);

    //if (m_currentIndexBuffer.valid() && ib.id == m_currentIndexBuffer.id)
    //    return;

    const auto &indexBuffer = m_indexBuffers[ib.id];
    DF3D_ASSERT(indexBuffer.gl_id != 0, "sanity check");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.gl_id);

    m_indexedDrawCall = true;
    m_currentIndexBuffer = ib;
}

void RenderBackendGL::updateIndexBuffer(IndexBufferDescriptor ib, size_t indicesCount, const void *data)
{
    DESCRIPTOR_CHECK(ib);

    const auto &indexBuffer = m_indexBuffers[ib.id];
    DF3D_ASSERT(indexBuffer.gl_id != 0, "sanity check");

    auto bytesUpdating = indicesCount * sizeof(INDICES_TYPE);

    DF3D_ASSERT(bytesUpdating <= indexBuffer.sizeInBytes, "sanity check");
    DF3D_ASSERT(indexBuffer.gl_id != 0, "sanity check");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.gl_id);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bytesUpdating, data);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_currentIndexBuffer = {};
}

df3d::TextureDescriptor RenderBackendGL::createTexture2D(int width, int height, PixelFormat format, const uint8_t *data, const TextureCreationParams &params)
{
    TextureDescriptor textureDescr = { m_texturesBag.getNew() };
    if (!textureDescr.valid())
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

    glGenTextures(1, &texture.gl_id);
    if (texture.gl_id == 0)
        return{};

    glBindTexture(GL_TEXTURE_2D, texture.gl_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    SetupGLWrapMode(GL_TEXTURE_2D, params.getWrapMode());
    SetupGLTextureFiltering(GL_TEXTURE_2D, params.getFiltering(), params.isMipmapped());

    // Init empty texture.
    if (format == PixelFormat::DEPTH)
        glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, pixelDataFormat, GL_FLOAT, nullptr);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, pixelDataFormat, GL_UNSIGNED_BYTE, nullptr);

    if (data)
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, pixelDataFormat, GL_UNSIGNED_BYTE, data);

    if (params.isMipmapped())
        glGenerateMipmap(GL_TEXTURE_2D);

    if (GL_EXT_texture_filter_anisotropic)
    {
        if (params.getAnisotropyLevel() != 1)
        {
            float aniso = m_caps.maxAnisotropy;
            if (params.getAnisotropyLevel() != render_constants::ANISOTROPY_LEVEL_MAX)
            {
                aniso = (float)params.getAnisotropyLevel();
            }

            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
        }
    }

    texture.sizeInBytes = width * height * GetPixelSizeForFormat(format);   // TODO: mipmaps!
    texture.type = GL_TEXTURE_2D;
    texture.pixelFormat = pixelDataFormat;

    glBindTexture(GL_TEXTURE_2D, 0);

    printOpenGLError();

    m_textures[textureDescr.id] = texture;

    m_stats.textures++;
    m_stats.textureMemoryBytes += texture.sizeInBytes;

    return textureDescr;
}

df3d::TextureDescriptor RenderBackendGL::createTextureCube(unique_ptr<PixelBuffer> pixels[(size_t)CubeFace::COUNT], const TextureCreationParams &params)
{
    TextureDescriptor textureDescr = { m_texturesBag.getNew() };
    if (!textureDescr.valid())
    {
        DFLOG_WARN("Failed to create a cube texture");
        return{};
    }

    TextureGL texture;

    glGenTextures(1, &texture.gl_id);
    if (texture.gl_id == 0)
        return{};

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.gl_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    SetupGLWrapMode(GL_TEXTURE_CUBE_MAP, params.getWrapMode());
    SetupGLTextureFiltering(GL_TEXTURE_CUBE_MAP, params.getFiltering(), params.isMipmapped());

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
            return false;
        }

        auto data = pixels[i]->getData();
        auto width = pixels[i]->getWidth();
        auto height = pixels[i]->getHeight();
        glTexImage2D(MapSidesToGL.find((CubeFace)i)->second, 0, glPixelFormat, width, height, 0, glPixelFormat, GL_UNSIGNED_BYTE, data);

        texture.sizeInBytes += pixels[i]->getSizeInBytes();
    }

    if (params.isMipmapped())
        glGenerateMipmap(GL_TEXTURE_2D);

    texture.type = GL_TEXTURE_CUBE_MAP;

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    printOpenGLError();

    m_textures[textureDescr.id] = texture;

    m_stats.textures++;
    m_stats.textureMemoryBytes += texture.sizeInBytes;

    return textureDescr;
}

void RenderBackendGL::updateTexture(TextureDescriptor t, int w, int h, const void *data)
{
    // FIXME: works only for 2d textures.
    DESCRIPTOR_CHECK(t);

    const auto &texture = m_textures[t.id];
    DF3D_ASSERT(texture.type != GL_INVALID_ENUM, "sanity check");

    glBindTexture(GL_TEXTURE_2D, texture.gl_id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, texture.pixelFormat, GL_UNSIGNED_BYTE, data);
}

void RenderBackendGL::destroyTexture(TextureDescriptor t)
{
    DESCRIPTOR_CHECK(t);

    const auto &texture = m_textures[t.id];
    if (texture.type == GL_INVALID_ENUM)
    {
        DF3D_ASSERT(false, "sanity check");
        return;
    }

    glBindTexture(texture.type, 0);
    if (texture.gl_id)
        glDeleteTextures(1, &texture.gl_id);

    DF3D_ASSERT(m_stats.textures > 0 && m_stats.textureMemoryBytes >= texture.sizeInBytes, "sanity check");
    m_stats.textures--;
    m_stats.textureMemoryBytes -= texture.sizeInBytes;

    m_textures[t.id] = {};

    m_texturesBag.release(t.id);
}

void RenderBackendGL::bindTexture(TextureDescriptor t, int unit)
{
    DESCRIPTOR_CHECK(t);

    const auto &texture = m_textures[t.id];
    DF3D_ASSERT(texture.gl_id != 0 && texture.type != GL_INVALID_ENUM, "sanity check");

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(texture.type, texture.gl_id);
}

df3d::ShaderDescriptor RenderBackendGL::createShader(ShaderType type, const std::string &data)
{
    if (data.empty())
    {
        DFLOG_WARN("Failed to create a shader: empty shader data");
        return{};
    }

    ShaderDescriptor shaderDescr = { m_shadersBag.getNew() };
    if (!shaderDescr.valid())
    {
        DFLOG_WARN("Failed to create a shader");
        return{};
    }

    ShaderGL shader;

    if (type == ShaderType::VERTEX)
    {
        shader.gl_id = glCreateShader(GL_VERTEX_SHADER);
        shader.type = GL_VERTEX_SHADER;
    }
    else if (type == ShaderType::FRAGMENT)
    {
        shader.gl_id = glCreateShader(GL_FRAGMENT_SHADER);
        shader.type = GL_FRAGMENT_SHADER;
    }

    if (shader.gl_id == 0)
    {
        DFLOG_WARN("Failed to create a shader");
        return{};
    }

    // Compile the shader.
    const char *pdata = data.c_str();
    glShaderSource(shader.gl_id, 1, &pdata, nullptr);
    glCompileShader(shader.gl_id);

    int compileOk;
    glGetShaderiv(shader.gl_id, GL_COMPILE_STATUS, &compileOk);
    if (compileOk == GL_FALSE)
    {
        DFLOG_WARN("Failed to compile a shader");
        DFLOG_MESS("\n\n%s\n\n", data.c_str());
        PrintShaderLog(shader.gl_id);

        m_shadersBag.release(shaderDescr.id);
        glDeleteShader(shader.gl_id);

        DEBUG_BREAK();

        return{};
    }

    printOpenGLError();

    m_shaders[shaderDescr.id] = shader;

    return shaderDescr;
}

void RenderBackendGL::destroyShader(ShaderDescriptor shader)
{
    DESCRIPTOR_CHECK(shader);

    const auto &shaderGL = m_shaders[shader.id];

    if (shaderGL.gl_id != 0)
        glDeleteShader(shaderGL.gl_id);
    else
        return;

    m_shaders[shader.id] = {};

    m_shadersBag.release(shader.id);
}

df3d::GpuProgramDescriptor RenderBackendGL::createGpuProgram(ShaderDescriptor vertexShader, ShaderDescriptor fragmentShader)
{
    DESCRIPTOR_CHECK_RETURN_INVALID(vertexShader);
    DESCRIPTOR_CHECK_RETURN_INVALID(fragmentShader);

    GpuProgramDescriptor programDescr = { m_gpuProgramsBag.getNew() };
    if (!programDescr.valid())
    {
        DFLOG_WARN("Failed to create a gpu program");
        return{};
    }

    ProgramGL program;

    program.gl_id = glCreateProgram();
    if (program.gl_id == 0)
    {
        DFLOG_WARN("Failed to create a GPU program");
        return{};
    }

    const auto &vertexShaderGL = m_shaders[vertexShader.id];
    const auto &fragmentShaderGL = m_shaders[fragmentShader.id];

    DF3D_ASSERT(vertexShaderGL.gl_id && fragmentShaderGL.gl_id, "sanity check");

    glAttachShader(program.gl_id, vertexShaderGL.gl_id);
    glAttachShader(program.gl_id, fragmentShaderGL.gl_id);

    // TODO: use VAO + refactor this.
    glBindAttribLocation(program.gl_id, VertexFormat::POSITION_3, "a_vertex3");
    glBindAttribLocation(program.gl_id, VertexFormat::NORMAL_3, "a_normal");
    glBindAttribLocation(program.gl_id, VertexFormat::TX_2, "a_txCoord");
    glBindAttribLocation(program.gl_id, VertexFormat::COLOR_4, "a_vertexColor");
    glBindAttribLocation(program.gl_id, VertexFormat::TANGENT_3, "a_tangent");
    glBindAttribLocation(program.gl_id, VertexFormat::BITANGENT_3, "a_bitangent");

    glLinkProgram(program.gl_id);

    int linkOk;
    glGetProgramiv(program.gl_id, GL_LINK_STATUS, &linkOk);
    if (linkOk == GL_FALSE)
    {
        DFLOG_WARN("GPU program linkage failed");
        PrintGpuProgramLog(program.gl_id);

        m_gpuProgramsBag.release(programDescr.id);
        glDeleteProgram(program.gl_id);

        return{};
    }

    printOpenGLError();

    m_programs[programDescr.id] = program;

    return programDescr;
}

void RenderBackendGL::destroyGpuProgram(GpuProgramDescriptor program)
{
    DESCRIPTOR_CHECK(program);

    const auto &programGL = m_programs[program.id];

    glUseProgram(0);
    if (programGL.gl_id != 0)
        glDeleteProgram(programGL.gl_id);
    else
        return;

    m_programs[program.id] = {};

    m_gpuProgramsBag.release(program.id);

    // Destroy associated uniforms
    {
        auto programUniformsFound = m_programUniforms.find(program.id);
        if (programUniformsFound != m_programUniforms.end())
        {
            for (auto uniDescr : programUniformsFound->second)
                m_uniformsBag.release(uniDescr.id);

            m_programUniforms.erase(programUniformsFound);
        }
    }
}

void RenderBackendGL::bindGpuProgram(GpuProgramDescriptor program)
{
    DESCRIPTOR_CHECK(program);

    if (program.id == m_currentProgram.id)
        return;

    const auto &programGL = m_programs[program.id];
    DF3D_ASSERT(programGL.gl_id != 0, "sanity check");

    glUseProgram(programGL.gl_id);

    m_currentProgram = program;
}

void RenderBackendGL::requestUniforms(GpuProgramDescriptor program, std::vector<UniformDescriptor> &outDescr, std::vector<std::string> &outNames)
{
    DESCRIPTOR_CHECK(program);

    const auto &programGL = m_programs[program.id];

    DF3D_ASSERT(programGL.gl_id != 0, "sanity check");

    int total = -1;
    glGetProgramiv(programGL.gl_id, GL_ACTIVE_UNIFORMS, &total);

    for (int i = 0; i < total; i++)
    {
        UniformDescriptor uniformDescr = { m_uniformsBag.getNew() };
        if (!uniformDescr.valid())
        {
            DFLOG_WARN("Failed to request uniforms");
            outDescr.clear();
            return;
        }

        outDescr.push_back(uniformDescr);
    }

    m_programUniforms[program.id] = {};
    auto programUniformsMap = m_programUniforms.find(program.id);

    for (int i = 0; i < total; i++)
    {
        GLenum type = GL_INVALID_ENUM;
        int nameLength = -1, uniformVarSize = -1;
        char name[100];

        glGetActiveUniform(programGL.gl_id, i, sizeof(name) - 1, &nameLength, &uniformVarSize, &type, name);
        name[nameLength] = 0;

        outNames.push_back(name);

        UniformGL uniformGL;
        uniformGL.type = type;
        uniformGL.location = glGetUniformLocation(programGL.gl_id, name);

        m_uniforms[outDescr[i].id] = uniformGL;

        programUniformsMap->second.push_back(outDescr[i]);
    }
}

void RenderBackendGL::setUniformValue(UniformDescriptor uniform, const void *data)
{
    DESCRIPTOR_CHECK(uniform);

    const auto &uniformGL = m_uniforms[uniform.id];

    DF3D_ASSERT(uniformGL.type != GL_INVALID_ENUM && uniformGL.location != -1, "sanity check");

    switch (uniformGL.type)
    {
    case GL_SAMPLER_2D:
        glUniform1iv(uniformGL.location, 1, (GLint *)data);
        break;
    case GL_SAMPLER_CUBE:
        glUniform1iv(uniformGL.location, 1, (GLint *)data);
        break;
    case GL_INT:
        glUniform1iv(uniformGL.location, 1, (GLint *)data);
        break;
    case GL_FLOAT:
        glUniform1fv(uniformGL.location, 1, (GLfloat *)data);
        break;
    case GL_FLOAT_VEC2:
        glUniform2fv(uniformGL.location, 1, (GLfloat *)data);
        break;
    case GL_FLOAT_VEC3:
        glUniform3fv(uniformGL.location, 1, (GLfloat *)data);
        break;
    case GL_FLOAT_VEC4:
        glUniform4fv(uniformGL.location, 1, (GLfloat *)data);
        break;
    case GL_FLOAT_MAT3:
        glUniformMatrix3fv(uniformGL.location, 1, GL_FALSE, (GLfloat *)data);
        break;
    case GL_FLOAT_MAT4:
        glUniformMatrix4fv(uniformGL.location, 1, GL_FALSE, (GLfloat *)data);
        break;
    default:
        DFLOG_WARN("Failed to update GpuProgramUniform. Unknown uniform type");
        break;
    }
}

void RenderBackendGL::setViewport(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);
}

void RenderBackendGL::clearColorBuffer(const glm::vec4 &color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RenderBackendGL::clearDepthBuffer()
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

void RenderBackendGL::clearStencilBuffer()
{
    glClear(GL_STENCIL_BUFFER_BIT);
}

void RenderBackendGL::enableDepthTest(bool enable)
{
    if (enable)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void RenderBackendGL::enableDepthWrite(bool enable)
{
    glDepthMask(enable);
}

void RenderBackendGL::enableScissorTest(bool enable)
{
    if (enable) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
}

void RenderBackendGL::setScissorRegion(int x, int y, int width, int height)
{
    glScissor(x, y, width, height);
}

void RenderBackendGL::setBlendingMode(BlendingMode mode)
{
    switch (mode)
    {
    case BlendingMode::NONE:
        glDisable(GL_BLEND);
        break;
    case BlendingMode::ADDALPHA:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        break;
    case BlendingMode::ALPHA:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case BlendingMode::ADD:
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        break;
    default:
        break;
    }
}

void RenderBackendGL::setCullFaceMode(FaceCullMode mode)
{
    switch (mode)
    {
    case FaceCullMode::NONE:
        glDisable(GL_CULL_FACE);
        break;
    case FaceCullMode::FRONT:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        break;
    case FaceCullMode::BACK:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        break;
    default:
        break;
    }
}

void RenderBackendGL::draw(RopType type, size_t numberOfElements)
{
    if (m_indexedDrawCall)
        glDrawElements(GetGLDrawMode(type), numberOfElements, GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(GetGLDrawMode(type), 0, numberOfElements);

    // Update stats.
#ifdef _DEBUG
    {
        m_stats.drawCalls++;
        switch (type)
        {
        case RopType::LINES:
            m_stats.totalLines += numberOfElements / 2;
            break;
        case RopType::TRIANGLES:
            m_stats.totalTriangles += numberOfElements / 3;
            break;
        default:
            break;
        }
    }
#endif
}

unique_ptr<IRenderBackend> IRenderBackend::create()
{
    return make_unique<RenderBackendGL>();
}

}
