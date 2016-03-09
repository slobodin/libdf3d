#include "RenderBackendGL.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/resources/ResourceManager.h>
#include <libdf3d/resources/ResourceFactory.h>
#include <libdf3d/render/RenderCommon.h>
#include <libdf3d/render/Texture.h>
#include <libdf3d/render/RenderOperation.h>
#include <libdf3d/render/GpuProgram.h>

namespace df3d {

static std::string CheckGLError()
{
#if defined(DF3D_DESKTOP)
    std::string errString;

    GLenum errCode = glGetError();
    if (errCode != GL_NO_ERROR)
        errString = (char *)gluErrorString(errCode);

    return errString;
#else
    GLenum errCode = glGetError();
    if (errCode == GL_NO_ERROR)
        return "";

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
        glog << "OpenGL error:" << err << ". File:" << file << ". Line:" << line << logwarn;
}

#if defined(_DEBUG) || defined(DEBUG)
#define printOpenGLError() CheckAndPrintGLError(__FILE__, __LINE__)
#else
#define printOpenGLError()
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

/*
static void SetupGlTextureFiltering(GLenum glType, TextureFiltering filtering, bool mipmapped)
{
    glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, filtering == TextureFiltering::NEAREST ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, getGlFilteringMode(filtering, mipmapped));

    printOpenGLError();
}

static void SetupGlWrapMode(GLenum glType, TextureWrapMode wrapMode)
{
    auto wmGl = getGlWrapMode(wrapMode);
    glTexParameteri(glType, GL_TEXTURE_WRAP_S, wmGl);
    glTexParameteri(glType, GL_TEXTURE_WRAP_T, wmGl);
#if defined(DF3D_DESKTOP)
    glTexParameteri(glType, GL_TEXTURE_WRAP_R, wmGl);
#endif

    printOpenGLError();
}


void VertexFormat::enableGLAttributes()
{
for (auto attrib : m_attribs)
{
glEnableVertexAttribArray(attrib);
size_t offs = getOffsetTo(attrib);
glVertexAttribPointer(attrib, m_counts[attrib], GL_FLOAT, GL_FALSE, getVertexSize(), (const GLvoid*)offs);
}
}

void VertexFormat::disableGLAttributes()
{
for (auto attrib : m_attribs)
glDisableVertexAttribArray(attrib);
}


*/

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

    glog << "Shader info log:" << infoLog.get() << logmess;
}

static void PrintGpuProgramLog(unsigned int program)
{
    int infologLen = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLen);

    unique_ptr<char> infoLog(new char[infologLen + 1]);
    glGetProgramInfoLog(program, infologLen, nullptr, infoLog.get());

    glog << "GPU program info log:" << infoLog.get() << logmess;
}

void RenderBackendGL::createWhiteTexture()
{
    const auto w = 8;
    const auto h = 8;
    const auto pf = PixelFormat::RGBA;

    auto data = new unsigned char[w * h * 4];
    memset(data, 255, w * h * 4);

    TextureCreationParams params;
    params.setFiltering(TextureFiltering::NEAREST);
    params.setMipmapped(false);
    params.setWrapMode(TextureWrapMode::WRAP);
    params.setAnisotropyLevel(NO_ANISOTROPY);

    auto pb = make_unique<PixelBuffer>(w, h, data, pf);

    m_whiteTexture = svc().resourceManager().getFactory().createTexture(std::move(pb), params);
    m_whiteTexture->setResident(true);

    delete[] data;
}

void RenderBackendGL::loadResidentGpuPrograms()
{
    const std::string simple_lighting_vert =
#include "embed_glsl/simple_lighting_vert.h"
        ;
    const std::string simple_lighting_frag =
#include "embed_glsl/simple_lighting_frag.h"
        ;
    const std::string colored_vert =
#include "embed_glsl/colored_vert.h"
        ;
    const std::string colored_frag =
#include "embed_glsl/colored_frag.h"
        ;
    const std::string ambient_vert =
#include "embed_glsl/ambient_vert.h"
        ;
    const std::string ambient_frag =
#include "embed_glsl/ambient_frag.h"
        ;

    auto &factory = svc().resourceManager().getFactory();

    factory.createGpuProgram(SIMPLE_LIGHTING_PROGRAM_EMBED_PATH, simple_lighting_vert, simple_lighting_frag)->setResident(true);
    factory.createGpuProgram(COLORED_PROGRAM_EMBED_PATH, colored_vert, colored_frag)->setResident(true);
    factory.createGpuProgram(AMBIENT_PASS_PROGRAM_EMBED_PATH, ambient_vert, ambient_frag)->setResident(true);
}

RenderBackendGL::RenderBackendGL()
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
        glog << "OpenGL version" << ver << logmess;

        const char *card = (const char *)glGetString(GL_RENDERER);
        const char *vendor = (const char *)glGetString(GL_VENDOR);
        glog << "Using" << card << vendor << logmess;

        const char *shaderVer = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
        glog << "Shaders version" << shaderVer << logmess;

        glog << "Max texture size" << m_caps.maxTextureSize << logmess;
    }

    printOpenGLError();
}

RenderBackendGL::~RenderBackendGL()
{

}

void RenderBackendGL::createEmbedResources()
{
    createWhiteTexture();
    loadResidentGpuPrograms();
}

const RenderBackendCaps& RenderBackendGL::getCaps() const
{
    return m_caps;
}

void RenderBackendGL::frameBegin()
{
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // TODO_render
    /*
    m_programState->onFrameBegin();
    */

    // Clear previous frame GL error.
    printOpenGLError();
}

void RenderBackendGL::frameEnd()
{
    glFlush();
}

df3d::VertexBufferDescriptor RenderBackendGL::createVertexBuffer(const VertexFormat &format, size_t verticesCount, const void *data, GpuBufferUsageType usage)
{
    //glGenBuffers(1, &vb.id);

    assert(verticesCount > 0);
    /*
    glBindBuffer(GL_ARRAY_BUFFER, m_glId);
    glBufferData(GL_ARRAY_BUFFER, verticesCount * m_format.getVertexSize(), data, getGLUsageType(usage));

    m_verticesUsed = verticesCount;
    m_sizeInBytes = verticesCount * m_format.getVertexSize();
    */

    return{};
}

void RenderBackendGL::destroyVertexBuffer(VertexBufferDescriptor vb)
{
    /*
    glDeleteBuffers(0, &glVb.id);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &m_glId);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &m_glId);
    */
}

void RenderBackendGL::bindVertexBuffer(VertexBufferDescriptor vb)
{
    /*
    glBindBuffer(GL_ARRAY_BUFFER, m_glId);

    m_format.enableGLAttributes();*/
    /*
    m_format.disableGLAttributes();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    */
}

void RenderBackendGL::updateVertexBuffer(VertexBufferDescriptor vb, size_t verticesCount, const void *data)
{
    /*
    auto bytesUpdating = verticesCount * m_format.getVertexSize();

    assert(bytesUpdating <= m_sizeInBytes);

    glBindBuffer(GL_ARRAY_BUFFER, m_glId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, bytesUpdating, data);
    */
}

df3d::IndexBufferDescriptor RenderBackendGL::createIndexBuffer(size_t indicesCount, const void *data, GpuBufferUsageType usage)
{/*
    glGenBuffers(1, &m_glId);

    assert(m_glId);
    */

    /*
    void IndexBuffer::alloc()
    {
        assert(indicesCount > 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(INDICES_TYPE), data, getGLUsageType(usage));

        m_indicesUsed = indicesCount;
        m_sizeInBytes = indicesCount * sizeof(INDICES_TYPE);
    }
    */

    return{};
}

void RenderBackendGL::destroyIndexBuffer(IndexBufferDescriptor ib)
{

}

void RenderBackendGL::bindIndexBuffer(IndexBufferDescriptor ib)
{/*
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glId);
    */
}

void RenderBackendGL::updateIndexBuffer(IndexBufferDescriptor ib, size_t indicesCount, const void *data)
{
    /*
    auto bytesUpdating = indicesCount * sizeof(INDICES_TYPE);

    assert(bytesUpdating <= m_sizeInBytes);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glId);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bytesUpdating, data);
    */
}

df3d::TextureDescriptor RenderBackendGL::createTexture2D(const PixelBuffer &pixels, const TextureCreationParams &params)
{
    /*
    m_originalWidth = buffer.getWidth();
    m_originalHeight = buffer.getHeight();

    auto actWidth = /*getNextPot(*buffer.getWidth();
    auto actHeight = /*getNextPot(*buffer.getHeight();

    auto maxSize = svc().renderManager().getRenderer()->getMaxTextureSize();
    if (actWidth > maxSize || actHeight > maxSize)
    {
        glog << "Failed to create texture. Size is too big." << logwarn;
        return false;
    }

    GLint glPixelFormat = 0;
    switch (buffer.getFormat())
    {
    case PixelFormat::RGB:
    case PixelFormat::BGR:
        glPixelFormat = GL_RGB;
        break;
    case PixelFormat::RGBA:
        glPixelFormat = GL_RGBA;
        break;
    case PixelFormat::GRAYSCALE:
        glPixelFormat = GL_LUMINANCE;   // FIXME: is it valid on ES?
        break;
    case PixelFormat::DEPTH:
        glPixelFormat = GL_DEPTH_COMPONENT16;
        break;
    default:
        glog << "Invalid GL texture pixel format" << logwarn;
        return false;
    }

    m_actualWidth = actWidth;
    m_actualHeight = actHeight;

    if (!(m_actualWidth == buffer.getWidth() && m_actualHeight == buffer.getHeight()))
        glog << "Texture with name" << getGUID() << "is not pot" << logdebug;

    glGenTextures(1, &m_glid);
    glBindTexture(GL_TEXTURE_2D, m_glid);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    setupGlWrapMode(GL_TEXTURE_2D, m_params.getWrapMode());
    setupGlTextureFiltering(GL_TEXTURE_2D, m_params.getFiltering(), m_params.isMipmapped());

    // FIXME:
    // Init empty texture.
    if (buffer.getFormat() == PixelFormat::DEPTH)
        glTexImage2D(GL_TEXTURE_2D, 0, glPixelFormat, m_actualWidth, m_actualHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, glPixelFormat, m_actualWidth, m_actualHeight, 0, glPixelFormat, GL_UNSIGNED_BYTE, nullptr);

    if (buffer.getData())
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, buffer.getWidth(), buffer.getHeight(), glPixelFormat, GL_UNSIGNED_BYTE, buffer.getData());

    if (m_params.isMipmapped())
        glGenerateMipmap(GL_TEXTURE_2D);

    if (GL_EXT_texture_filter_anisotropic)
    {
        if (m_params.getAnisotropyLevel() != 1)
        {
            float aniso = svc().renderManager().getRenderer()->getMaxAnisotropy();
            if (m_params.getAnisotropyLevel() != ANISOTROPY_LEVEL_MAX)
            {
                aniso = (float)m_params.getAnisotropyLevel();
            }

            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
        }
    }

    m_sizeInBytes = buffer.getSizeInBytes();        // TODO: mipmaps!

    glBindTexture(GL_TEXTURE_2D, 0);

    printOpenGLError();

    svc().getFrameStats().addTexture(*this);
    */

    TextureDescriptor d;
    d.id = 1;

    return d;
}

df3d::TextureDescriptor RenderBackendGL::createTextureCube(unique_ptr<PixelBuffer> pixels[(size_t)CubeFace::COUNT], const TextureCreationParams &params)
{/*
    m_sizeInBytes = 0;

    glGenTextures(1, &m_glid);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_glid);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    setupGlWrapMode(GL_TEXTURE_CUBE_MAP, m_params.getWrapMode());
    setupGlTextureFiltering(GL_TEXTURE_CUBE_MAP, m_params.getFiltering(), m_params.isMipmapped());

    for (int i = 0; i < (size_t)CubeFace::COUNT; i++)
    {
        GLint glPixelFormat = 0;
        switch (images[i]->getFormat())
        {
        case PixelFormat::RGB:
        case PixelFormat::BGR:
            glPixelFormat = GL_RGB;
            break;
        case PixelFormat::RGBA:
            glPixelFormat = GL_RGBA;
            break;
        default:
            glog << "Invalid GL texture pixel format" << logwarn;
            return false;
        }

        auto data = images[i]->getData();
        auto width = images[i]->getWidth();
        auto height = images[i]->getHeight();
        glTexImage2D(MapSidesToGl.find((CubeFace)i)->second, 0, glPixelFormat, width, height, 0, glPixelFormat, GL_UNSIGNED_BYTE, data);

        m_sizeInBytes += images[i]->getSizeInBytes();
    }

    if (m_params.isMipmapped())
        glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    svc().getFrameStats().addTexture(*this);

    printOpenGLError();

    return true;*/
    return{};
}

void RenderBackendGL::destroyTexture(TextureDescriptor t)
{
    /*
    if (m_glid)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &m_glid);

        m_glid = 0;

        svc().getFrameStats().removeTexture(*this);
    }*/
    /*
    if (m_glid)
    {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glDeleteTextures(1, &m_glid);

    svc().getFrameStats().removeTexture(*this);

    m_glid = 0;
    }
    */
}

void RenderBackendGL::bindTexture(TextureDescriptor t)
{
    /*
    if (!isInitialized())
        return false;

    assert(m_glid);

    glBindTexture(GL_TEXTURE_2D, m_glid);

    return true;
    */
    /*
    if (!isInitialized())
    return false;

    assert(m_glid);

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_glid);

    return true;*/
}

df3d::ShaderDescriptor RenderBackendGL::createShader(ShaderType type, const std::string &data)
{
    /*
            void Shader::createGLShader()
            {
                if (m_type == Type::VERTEX)
                    m_shaderDescriptor = glCreateShader(GL_VERTEX_SHADER);
                else if (m_type == Type::FRAGMENT)
                    m_shaderDescriptor = glCreateShader(GL_FRAGMENT_SHADER);
                else
                    m_shaderDescriptor = 0;
            }



            bool Shader::compile()
            {
                if (m_isCompiled)
                    return true;

                // Try to create shader.
                if (m_shaderDescriptor == 0)
                    createGLShader();

                if (m_shaderDescriptor == 0 || m_type == Type::UNDEFINED)
                {
                    glog << "Can not compile GLSL shader due to undefined type" << logwarn;
                    return false;
                }

                if (m_shaderData.empty())
                {
                    glog << "Empty shader data" << logwarn;
                    return false;
                }

                const char *pdata = m_shaderData.c_str();
                glShaderSource(m_shaderDescriptor, 1, &pdata, nullptr);
                glCompileShader(m_shaderDescriptor);

                int compileOk;
                glGetShaderiv(m_shaderDescriptor, GL_COMPILE_STATUS, &compileOk);
                if (compileOk == GL_FALSE)
                {
                    glog << "Failed to compile a shader" << logwarn;
                    glog << "\n" << m_shaderData << logmess;
                    shaderLog(m_shaderDescriptor);

                    return false;
                }

                setCompiled(true);

                return true;
            }

            void Shader::setShaderData(const std::string &data)
            {
                m_shaderData = data;
                setCompiled(false);
            }

            Shader::Shader(Type type)
                : m_type(type)
            {
            }

            Shader::~Shader()
            {
                if (m_shaderDescriptor != 0)
                    glDeleteShader(m_shaderDescriptor);
            }

            shared_ptr<Shader> Shader::createFromFile(const std::string &filePath)
            {

            }

            shared_ptr<Shader> Shader::createFromString(const std::string &shaderData, Type type)
            {

            }

            }
*/

    ShaderDescriptor d;
    d.id = 1;

    return d;
}

void RenderBackendGL::destroyShader(ShaderDescriptor shader)
{

}

df3d::GpuProgramDescriptor RenderBackendGL::createGpuProgram(ShaderDescriptor vertexShader, ShaderDescriptor fragmentShader)
{
    /*
    bool GpuProgram::compileShaders()
    {
        assert(!m_programDescriptor);

        m_programDescriptor = glCreateProgram();

        for (auto shader : m_shaders)
        {
            if (!shader || !shader->compile())
            {
                glog << "Failed to compile shaders in" << getGUID() << logwarn;
                return false;
            }
        }

        return true;
    }

    bool GpuProgram::attachShaders()
    {
        if (!m_programDescriptor)
            return false;

        for (auto shader : m_shaders)
            glAttachShader(m_programDescriptor, shader->m_shaderDescriptor);

        glBindAttribLocation(m_programDescriptor, VertexFormat::POSITION_3, "a_vertex3");
        glBindAttribLocation(m_programDescriptor, VertexFormat::NORMAL_3, "a_normal");
        glBindAttribLocation(m_programDescriptor, VertexFormat::TX_2, "a_txCoord");
        glBindAttribLocation(m_programDescriptor, VertexFormat::COLOR_4, "a_vertexColor");
        glBindAttribLocation(m_programDescriptor, VertexFormat::TANGENT_3, "a_tangent");
        glBindAttribLocation(m_programDescriptor, VertexFormat::BITANGENT_3, "a_bitangent");

        glLinkProgram(m_programDescriptor);

        int linkOk;
        glGetProgramiv(m_programDescriptor, GL_LINK_STATUS, &linkOk);
        if (linkOk == GL_FALSE)
        {
            glog << "GPU program linkage failed" << logwarn;
            gpuProgramLog(m_programDescriptor);
            return false;
        }

        requestUniforms();

        printOpenGLError();

        return true;
    }

    void GpuProgram::requestUniforms()
    {
        int total = -1;
        glGetProgramiv(m_programDescriptor, GL_ACTIVE_UNIFORMS, &total);

        for (int i = 0; i < total; i++)
        {
            GLenum type = GL_ZERO;
            int nameLength = -1, uniformVarSize = -1;
            char name[100];

            glGetActiveUniform(m_programDescriptor, i, sizeof(name) - 1, &nameLength, &uniformVarSize, &type, name);
            name[nameLength] = 0;

            GpuProgramUniform uni(name);
            uni.m_location = glGetUniformLocation(m_programDescriptor, name);
            uni.m_glType = type;
            uni.m_isSampler = isSampler(type);

            if (uni.isShared())
                m_sharedUniforms.push_back(uni);
            else if (uni.isSampler())
                m_samplerUniforms.push_back(uni);
            else
                m_customUniforms.push_back(uni);
        }
    }

    GpuProgram::GpuProgram(GpuProgramDescriptor descr)
        : m_descriptor(descr)
    {
    }

    GpuProgram::~GpuProgram()
    {

    }

    void GpuProgram::bind()
    {
        if (!isInitialized())
            return;

        assert(m_programDescriptor);

        glUseProgram(m_programDescriptor);
    }

    void GpuProgram::unbind()
    {
        glUseProgram(0);
    }

    GpuProgramUniform *GpuProgram::getCustomUniform(const std::string &name)
    {
        for (size_t i = 0; i < m_customUniforms.size(); i++)
        {
            if (m_customUniforms[i].getName() == name)
                return &m_customUniforms[i];
        }

        return nullptr;
    }

    GpuProgramUniform* GpuProgram::getSamplerUniform(const std::string &name)
    {
        for (size_t i = 0; i < m_samplerUniforms.size(); i++)
        {
            if (m_samplerUniforms[i].getName() == name)
                return &m_samplerUniforms[i];
        }

        return nullptr;
    }

    */

GpuProgramDescriptor d;
d.id = 1;

return d;
}

void RenderBackendGL::destroyGpuProgram(GpuProgramDescriptor program)
{
    /*
    if (m_programDescriptor == 0)
        return;

    unbind();
    useProgram(0);!

    for (auto shader : m_shaders)
    {
        if (shader->m_shaderDescriptor != 0)
            glDetachShader(m_programDescriptor, shader->m_shaderDescriptor);
    }

    glDeleteProgram(m_programDescriptor);
    */
}

void RenderBackendGL::setViewport(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);

    // TODO_render:
    /*
    m_programState->m_pixelSize = glm::vec2(1.0f / (float)viewport.width(), 1.0f / (float)viewport.height());
    */
}

void RenderBackendGL::setWorldMatrix(const glm::mat4 &worldm)
{

}

void RenderBackendGL::setCameraMatrix(const glm::mat4 &viewm)
{

}

void RenderBackendGL::setProjectionMatrix(const glm::mat4 &projm)
{

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
    case FaceCullMode::FRONT_AND_BACK:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT_AND_BACK);
        break;
    default:
        break;
    }
}

void RenderBackendGL::draw(RopType type, size_t numberOfElements)
{
    // TODO_render

    /*
    if (!vb)
        return;

    // FIXME: figure out why crashes on NVIDIA
    //if (m_prevVB != vb)
    {
        vb->bind();
        m_prevVB = vb;
    }

    if (ib != nullptr)
    {
        ib->bind();
        glDrawElements(convertRopType(type), ib->getIndicesUsed(), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(convertRopType(type), 0, vb->getVerticesUsed());
    }

    svc().getFrameStats().addRenderOperation(vb, ib, type);

    printOpenGLError();
    */
}

unique_ptr<IRenderBackend> IRenderBackend::create()
{
    return make_unique<RenderBackendGL>();
}

}




//glUseProgram(0); --> FRAME END

























/*
shared_ptr<VertexBuffer> createQuad(const VertexFormat &vf, float x, float y, float w, float h, GpuBufferUsageType usage)
{
    // TODO_REFACTO remove this shit!!!

    float w2 = w / 2.0f;
    float h2 = h / 2.0f;

    float quad_pos[][2] =
    {
        { x - w2, y - h2 },
        { x + w2, y - h2 },
        { x + w2, y + h2 },
        { x + w2, y + h2 },
        { x - w2, y + h2 },
        { x - w2, y - h2 }
    };
    float quad_uv[][2] =
    {
        { 0.0, 0.0 },
        { 1.0, 0.0 },
        { 1.0, 1.0 },
        { 1.0, 1.0 },
        { 0.0, 1.0 },
        { 0.0, 0.0 }
    };

    VertexData vertexData(vf);

    for (int i = 0; i < 6; i++)
    {
        auto v = vertexData.allocVertex();

        v.setPosition({ quad_pos[i][0], quad_pos[i][1], 0.0f });
        v.setTx({ quad_uv[i][0], quad_uv[i][1] });
    }

    auto result = make_shared<VertexBuffer>(vf);
    result->alloc(vertexData, usage);

    return result;
}

unique_ptr<VertexBuffer> createQuad2(const VertexFormat &vf, float x, float y, float w, float h, GpuBufferUsageType usage)
{
    // TODO_REFACTO remove this shit!!!

    float w2 = w / 2.0f;
    float h2 = h / 2.0f;

    float quad_pos[][2] =
    {
        { x - w2, y - h2 },
        { x + w2, y - h2 },
        { x + w2, y + h2 },
        { x + w2, y + h2 },
        { x - w2, y + h2 },
        { x - w2, y - h2 }
    };
    float quad_uv[][2] =
    {
        { 0.0, 0.0 },
        { 1.0, 0.0 },
        { 1.0, 1.0 },
        { 1.0, 1.0 },
        { 0.0, 1.0 },
        { 0.0, 0.0 }
    };

    VertexData vertexData(vf);

    for (int i = 0; i < 6; i++)
    {
        auto v = vertexData.allocVertex();

        v.setPosition({ quad_pos[i][0], quad_pos[i][1], 0.0f });
        v.setTx({ quad_uv[i][0], quad_uv[i][1] });
        v.setColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    }

    auto result = make_unique<VertexBuffer>(vf);
    result->alloc(vertexData, usage);

    return result;
}
*/









/*

enum class SharedUniformType
{
    WORLD_VIEW_PROJECTION_MATRIX_UNIFORM,
    WORLD_VIEW_MATRIX_UNIFORM,
    WORLD_VIEW_3X3_MATRIX_UNIFORM,
    VIEW_INVERSE_MATRIX_UNIFORM,
    VIEW_MATRIX_UNIFORM,
    WORLD_INVERSE_MATRIX_UNIFORM,
    WORLD_MATRIX_UNIFORM,
    NORMAL_MATRIX_UNIFORM,
    PROJECTION_MATRIX_UNIFORM,

    CAMERA_POSITION_UNIFORM,

    GLOBAL_AMBIENT_UNIFORM,

    FOG_DENSITY_UNIFORM,
    FOG_COLOR_UNIFORM,

    PIXEL_SIZE_UNIFORM,

    ELAPSED_TIME_UNIFORM,

    MATERIAL_AMBIENT_UNIFORM,
    MATERIAL_DIFFUSE_UNIFORM,
    MATERIAL_SPECULAR_UNIFORM,
    MATERIAL_EMISSIVE_UNIFORM,
    MATERIAL_SHININESS_UNIFORM,

    SCENE_LIGHT_DIFFUSE_UNIFORM,
    SCENE_LIGHT_SPECULAR_UNIFORM,
    SCENE_LIGHT_POSITION_UNIFORM,
    SCENE_LIGHT_KC_UNIFORM,
    SCENE_LIGHT_KL_UNIFORM,
    SCENE_LIGHT_KQ_UNIFORM,

    COUNT
};

class GpuProgramUniform
{
    friend class GpuProgram;

    std::string m_name;
    int m_location = -1;
    unsigned m_glType = 0;

    SharedUniformType m_sharedId = SharedUniformType::COUNT;
    bool m_isSampler = false;

    GpuProgramUniform(const std::string &name);

public:
    ~GpuProgramUniform();

    void update(const void *data) const;

    SharedUniformType getSharedType() const { return m_sharedId; }
    bool isShared() const { return m_sharedId != SharedUniformType::COUNT; }
    bool isSampler() const { return m_isSampler; }
    const std::string &getName() const { return m_name; }
};

}










namespace df3d {

SharedUniformType getSharedTypeForUniform(const std::string &name)
{
    if (name == "u_worldViewProjectionMatrix")
        return SharedUniformType::WORLD_VIEW_PROJECTION_MATRIX_UNIFORM;
    else if (name == "u_worldViewMatrix")
        return SharedUniformType::WORLD_VIEW_MATRIX_UNIFORM;
    else if (name == "u_worldViewMatrix3x3")
        return SharedUniformType::WORLD_VIEW_3X3_MATRIX_UNIFORM;
    else if (name == "u_viewMatrixInverse")
        return SharedUniformType::VIEW_INVERSE_MATRIX_UNIFORM;
    else if (name == "u_viewMatrix")
        return SharedUniformType::VIEW_MATRIX_UNIFORM;
    else if (name == "u_projectionMatrix")
        return SharedUniformType::PROJECTION_MATRIX_UNIFORM;
    else if (name == "u_worldMatrix")
        return SharedUniformType::WORLD_MATRIX_UNIFORM;
    else if (name == "u_worldMatrixInverse")
        return SharedUniformType::WORLD_INVERSE_MATRIX_UNIFORM;
    else if (name == "u_normalMatrix")
        return SharedUniformType::NORMAL_MATRIX_UNIFORM;
    else if (name == "u_globalAmbient")
        return SharedUniformType::GLOBAL_AMBIENT_UNIFORM;
    else if (name == "u_cameraPosition")
        return SharedUniformType::CAMERA_POSITION_UNIFORM;
    else if (name == "u_fogDensity")
        return SharedUniformType::FOG_DENSITY_UNIFORM;
    else if (name == "u_fogColor")
        return SharedUniformType::FOG_COLOR_UNIFORM;
    else if (name == "u_pixelSize")
        return SharedUniformType::PIXEL_SIZE_UNIFORM;
    else if (name == "u_elapsedTime")
        return SharedUniformType::ELAPSED_TIME_UNIFORM;
    else if (name == "material.ambient")
        return SharedUniformType::MATERIAL_AMBIENT_UNIFORM;
    else if (name == "material.diffuse")
        return SharedUniformType::MATERIAL_DIFFUSE_UNIFORM;
    else if (name == "material.specular")
        return SharedUniformType::MATERIAL_SPECULAR_UNIFORM;
    else if (name == "material.emissive")
        return SharedUniformType::MATERIAL_EMISSIVE_UNIFORM;
    else if (name == "material.shininess")
        return SharedUniformType::MATERIAL_SHININESS_UNIFORM;
    else if (name == "current_light.diffuse")
        return SharedUniformType::SCENE_LIGHT_DIFFUSE_UNIFORM;
    else if (name == "current_light.specular")
        return SharedUniformType::SCENE_LIGHT_SPECULAR_UNIFORM;
    else if (name == "current_light.position")
        return SharedUniformType::SCENE_LIGHT_POSITION_UNIFORM;
    else if (name == "current_light.constantAttenuation")
        return SharedUniformType::SCENE_LIGHT_KC_UNIFORM;
    else if (name == "current_light.linearAttenuation")
        return SharedUniformType::SCENE_LIGHT_KL_UNIFORM;
    else if (name == "current_light.quadraticAttenuation")
        return SharedUniformType::SCENE_LIGHT_KQ_UNIFORM;

    return SharedUniformType::COUNT;
}

GpuProgramUniform::GpuProgramUniform(const std::string &name)
    : m_name(name)
{
    assert(!m_name.empty());

    // Try to set it shared.
    m_sharedId = getSharedTypeForUniform(m_name);
}

GpuProgramUniform::~GpuProgramUniform()
{

}

void GpuProgramUniform::update(const void *data) const
{
    switch (m_glType)
    {
    case GL_SAMPLER_2D:
        glUniform1iv(m_location, 1, (GLint *)data);
        break;
    case GL_SAMPLER_CUBE:
        glUniform1iv(m_location, 1, (GLint *)data);
        break;
    case GL_INT:
        glUniform1iv(m_location, 1, (GLint *)data);
        break;
    case GL_FLOAT:
        glUniform1fv(m_location, 1, (GLfloat *)data);
        break;
    case GL_FLOAT_VEC2:
        glUniform2fv(m_location, 1, (GLfloat *)data);
        break;
    case GL_FLOAT_VEC3:
        glUniform3fv(m_location, 1, (GLfloat *)data);
        break;
    case GL_FLOAT_VEC4:
        glUniform4fv(m_location, 1, (GLfloat *)data);
        break;
    case GL_FLOAT_MAT3:
        glUniformMatrix3fv(m_location, 1, GL_FALSE, (GLfloat *)data);
        break;
    case GL_FLOAT_MAT4:
        glUniformMatrix4fv(m_location, 1, GL_FALSE, (GLfloat *)data);
        break;
    default:
        glog << "Failed to update GpuProgramUniform. Unknown uniform type" << logwarn;
        break;
    }
}

}



*/








//////////////////////////////////////////////////////////////////////////
// GPU PROGRAM STATE

/*
#pragma once

namespace df3d {

class GpuProgramUniform;
class RenderPass;
class GpuProgram;

class GpuProgramState
{
    glm::mat4 m_worldMatrix;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projMatrix;

    glm::mat4 m_worldViewProj;
    glm::mat4 m_worldView;
    glm::mat3 m_worldView3x3;
    glm::mat3 m_normalMatrix;

    glm::mat4 m_viewMatrixInverse;
    glm::mat4 m_worldMatrixInverse;

    glm::vec3 m_cameraPosition;

    struct GLSLLight
    {
        glm::vec3 diffuseParam;
        glm::vec3 specularParam;
        // NOTE: this vector is translated to the View Space. See OpenGLRenderer::setLight.
        glm::vec4 positionParam;
        float k0Param = 1.0f;
        float k1Param = 1.0f;
        float k2Param = 1.0f;
    };

    GLSLLight m_currentLight;
    glm::vec4 m_globalAmbient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

    float m_fogDensity = 0.0f;
    glm::vec3 m_fogColor = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec2 m_pixelSize = glm::vec2(1.0f / (float)DEFAULT_WINDOW_WIDTH, 1.0f / (float)DEFAULT_WINDOW_HEIGHT);

    float m_engineElapsedTime = 0.0f;

    bool m_worldViewProjDirty = true;
    bool m_worldViewDirty = true;
    bool m_worldView3x3Dirty = true;
    bool m_normalDirty = true;
    bool m_viewInverseDirty = true;
    bool m_worldInverseDirty = true;

    void resetFlags();

    RenderPass *m_currentPass = nullptr;
    GpuProgram *m_currentShader = nullptr;

public:
    GpuProgramState() = default;
    ~GpuProgramState() = default;

    const glm::mat4& getWorldMatrix();
    const glm::mat4& getViewMatrix();
    const glm::mat4& getProjectionMatrix();
    const glm::mat4& getWorldViewProjectionMatrix();
    const glm::mat4& getWorldViewMatrix();
    const glm::mat3& getWorldView3x3Matrix();
    const glm::mat3& getNormalMatrix();
    const glm::mat4& getViewMatrixInverse();
    const glm::mat4& getWorldMatrixInverse();

    void setWorldMatrix(const glm::mat4 &worldm);
    void setViewMatrix(const glm::mat4 &viewm);
    void setProjectionMatrix(const glm::mat4 &projm);

    void onFrameBegin();
    void onFrameEnd();

    void updateSharedUniform(const GpuProgramUniform &uniform);
};

}
*/

/*
#include "GpuProgramState.h"

#include "GpuProgramUniform.h"
#include "RenderPass.h"
#include <libdf3d/base/EngineController.h>
#include <libdf3d/base/TimeManager.h>
#include <libdf3d/3d/Camera.h>
#include <libdf3d/game/World.h>

namespace df3d {

void GpuProgramState::resetFlags()
{
    m_worldViewProjDirty = m_worldViewDirty = m_worldView3x3Dirty
        = m_normalDirty = m_viewInverseDirty = m_worldInverseDirty = true;
}

const glm::mat4 &GpuProgramState::getWorldMatrix()
{
    return m_worldMatrix;
}

const glm::mat4 &GpuProgramState::getViewMatrix()
{
    return m_viewMatrix;
}

const glm::mat4 &GpuProgramState::getProjectionMatrix()
{
    return m_projMatrix;
}

const glm::mat4 &GpuProgramState::getWorldViewProjectionMatrix()
{
    if (m_worldViewProjDirty)
    {
        m_worldViewProj = m_projMatrix * getWorldViewMatrix();
        m_worldViewProjDirty = false;
    }

    return m_worldViewProj;
}

const glm::mat4 &GpuProgramState::getWorldViewMatrix()
{
    if (m_worldViewDirty)
    {
        m_worldView = m_viewMatrix * m_worldMatrix;
        m_worldViewDirty = false;
    }

    return m_worldView;
}

const glm::mat3 &GpuProgramState::getWorldView3x3Matrix()
{
    if (m_worldView3x3Dirty)
    {
        m_worldView3x3 = glm::mat3(getWorldViewMatrix());
        m_worldView3x3Dirty = false;
    }

    return m_worldView3x3;
}

const glm::mat3 &GpuProgramState::getNormalMatrix()
{
    if (m_normalDirty)
    {
        m_normalMatrix = glm::inverseTranspose(getWorldView3x3Matrix());
        m_normalDirty = false;
    }

    return m_normalMatrix;
}

const glm::mat4 &GpuProgramState::getViewMatrixInverse()
{
    if (m_viewInverseDirty)
    {
        m_viewMatrixInverse = glm::inverse(m_viewMatrix);
        m_viewInverseDirty = false;
    }

    return m_viewMatrixInverse;
}

const glm::mat4 &GpuProgramState::getWorldMatrixInverse()
{
    if (m_worldInverseDirty)
    {
        m_worldMatrixInverse = glm::inverse(m_worldMatrix);
        m_worldInverseDirty = false;
    }

    return m_worldMatrixInverse;
}

void GpuProgramState::setWorldMatrix(const glm::mat4 &worldm)
{
    m_worldMatrix = worldm;
    resetFlags();
}

void GpuProgramState::setViewMatrix(const glm::mat4 &viewm)
{
    m_viewMatrix = viewm;
    resetFlags();
}

void GpuProgramState::setProjectionMatrix(const glm::mat4 &projm)
{
    m_projMatrix = projm;
    // FIXME:
    // Not all flags have to be set. Whatever.
    resetFlags();
}

void GpuProgramState::onFrameBegin()
{
    resetFlags();

    m_worldMatrix = glm::mat4(1.0f);
    m_viewMatrix = glm::mat4(1.0f);
    m_projMatrix = glm::mat4(1.0f);

    m_currentPass = nullptr;
    m_currentShader = nullptr;

    m_cameraPosition = svc().world().getCamera()->getPosition();

    m_engineElapsedTime = svc().timer().getElapsedTime();
}

void GpuProgramState::onFrameEnd()
{

}

void GpuProgramState::updateSharedUniform(const GpuProgramUniform &uniform)
{
    // TODO: kind of lookup table.
    switch (uniform.getSharedType())
    {
    case SharedUniformType::WORLD_VIEW_PROJECTION_MATRIX_UNIFORM:
        uniform.update(glm::value_ptr(getWorldViewProjectionMatrix()));
        break;
    case SharedUniformType::WORLD_VIEW_MATRIX_UNIFORM:
        uniform.update(glm::value_ptr(getWorldViewMatrix()));
        break;
    case SharedUniformType::WORLD_VIEW_3X3_MATRIX_UNIFORM:
        uniform.update(glm::value_ptr(getWorldView3x3Matrix()));
        break;
    case SharedUniformType::VIEW_INVERSE_MATRIX_UNIFORM:
        uniform.update(glm::value_ptr(getViewMatrixInverse()));
        break;
    case SharedUniformType::VIEW_MATRIX_UNIFORM:
        uniform.update(glm::value_ptr(getViewMatrix()));
        break;
    case SharedUniformType::WORLD_INVERSE_MATRIX_UNIFORM:
        uniform.update(glm::value_ptr(getWorldMatrixInverse()));
        break;
    case SharedUniformType::WORLD_MATRIX_UNIFORM:
        uniform.update(glm::value_ptr(getWorldMatrix()));
        break;
    case SharedUniformType::PROJECTION_MATRIX_UNIFORM:
        uniform.update(glm::value_ptr(getProjectionMatrix()));
        break;
    case SharedUniformType::NORMAL_MATRIX_UNIFORM:
        uniform.update(glm::value_ptr(getNormalMatrix()));
        break;
    case SharedUniformType::CAMERA_POSITION_UNIFORM:
        uniform.update(glm::value_ptr(m_cameraPosition));
        break;
    case SharedUniformType::GLOBAL_AMBIENT_UNIFORM:
        uniform.update(glm::value_ptr(m_globalAmbient));
        break;
    case SharedUniformType::FOG_DENSITY_UNIFORM:
        uniform.update(&m_fogDensity);
        break;
    case SharedUniformType::FOG_COLOR_UNIFORM:
        uniform.update(glm::value_ptr(m_fogColor));
        break;
    case SharedUniformType::PIXEL_SIZE_UNIFORM:
        uniform.update(glm::value_ptr(m_pixelSize));
        break;
    case SharedUniformType::ELAPSED_TIME_UNIFORM:
        uniform.update(&m_engineElapsedTime);
        break;
    case SharedUniformType::MATERIAL_AMBIENT_UNIFORM:
        uniform.update(glm::value_ptr(m_currentPass->getAmbientColor()));
        break;
    case SharedUniformType::MATERIAL_DIFFUSE_UNIFORM:
        uniform.update(glm::value_ptr(m_currentPass->getDiffuseColor()));
        break;
    case SharedUniformType::MATERIAL_SPECULAR_UNIFORM:
        uniform.update(glm::value_ptr(m_currentPass->getSpecularColor()));
        break;
    case SharedUniformType::MATERIAL_EMISSIVE_UNIFORM:
        uniform.update(glm::value_ptr(m_currentPass->getEmissiveColor()));
        break;
    case SharedUniformType::MATERIAL_SHININESS_UNIFORM:
        uniform.update(&m_currentPass->getShininess());
        break;
    case SharedUniformType::SCENE_LIGHT_DIFFUSE_UNIFORM:
        uniform.update(glm::value_ptr(m_currentLight.diffuseParam));
        break;
    case SharedUniformType::SCENE_LIGHT_SPECULAR_UNIFORM:
        uniform.update(glm::value_ptr(m_currentLight.specularParam));
        break;
    case SharedUniformType::SCENE_LIGHT_POSITION_UNIFORM:
        uniform.update(glm::value_ptr(m_currentLight.positionParam));
        break;
    case SharedUniformType::SCENE_LIGHT_KC_UNIFORM:
        uniform.update(&m_currentLight.k0Param);
        break;
    case SharedUniformType::SCENE_LIGHT_KL_UNIFORM:
        uniform.update(&m_currentLight.k1Param);
        break;
    case SharedUniformType::SCENE_LIGHT_KQ_UNIFORM:
        uniform.update(&m_currentLight.k2Param);
        break;
    case SharedUniformType::COUNT:
    default:
        glog << "Can not set shared value to not shared uniform" << logwarn;
        break;
    }
}

}
*/




