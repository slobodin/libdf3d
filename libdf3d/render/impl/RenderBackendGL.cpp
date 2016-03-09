#include "RenderBackendGL.h"

#include <libdf3d/render/RenderCommon.h>

namespace df3d {

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
void SetupGlTextureFiltering(GLenum glType, TextureFiltering filtering, bool mipmapped)
{
    glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, filtering == TextureFiltering::NEAREST ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, getGlFilteringMode(filtering, mipmapped));

    printOpenGLError();
}

void SetupGlWrapMode(GLenum glType, TextureWrapMode wrapMode)
{
    auto wmGl = getGlWrapMode(wrapMode);
    glTexParameteri(glType, GL_TEXTURE_WRAP_S, wmGl);
    glTexParameteri(glType, GL_TEXTURE_WRAP_T, wmGl);
#if defined(DF3D_DESKTOP)
    glTexParameteri(glType, GL_TEXTURE_WRAP_R, wmGl);
#endif

    printOpenGLError();
}
*/

GLenum GetGLBufferUsageType(GpuBufferUsageType t)
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

void RenderBackendGL::bindVertexBuffer(VertexBufferDescriptor vb, size_t first, size_t count)
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

void RenderBackendGL::bindIndexBuffer(IndexBufferDescriptor ib, size_t first, size_t count)
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
    return{};
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

df3d::ShaderDescriptor RenderBackendGL::createShader()
{
    return{};
}

void RenderBackendGL::destroyShader()
{

}

df3d::GpuProgramDescriptor RenderBackendGL::createGpuProgram(ShaderDescriptor, ShaderDescriptor)
{
    return{};
}

void RenderBackendGL::destroyGpuProgram(GpuProgramDescriptor)
{

}

void RenderBackendGL::setViewport()
{

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

}

void RenderBackendGL::clearDepthBuffer()
{

}

void RenderBackendGL::clearStencilBuffer()
{

}

void RenderBackendGL::enableDepthTest(bool enable)
{

}

void RenderBackendGL::enableDepthWrite(bool enable)
{

}

void RenderBackendGL::enableScissorTest(bool enable)
{

}

void RenderBackendGL::setScissorRegion(int x, int y, int width, int height)
{

}

void RenderBackendGL::draw()
{

}

}






























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
