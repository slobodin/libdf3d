#include "Texture2D.h"

#include <base/Service.h>
#include "RendererBackend.h"

namespace df3d {

bool Texture2D::createGLTexture(const PixelBuffer &buffer)
{
    m_originalWidth = buffer.getWidth();
    m_originalHeight = buffer.getHeight();

    auto actWidth = /*getNextPot(*/buffer.getWidth();
    auto actHeight = /*getNextPot(*/buffer.getHeight();

    auto maxSize = svc().renderMgr.getRenderer()->getMaxTextureSize();
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
    glTexImage2D(GL_TEXTURE_2D, 0, glPixelFormat, m_actualWidth, m_actualHeight, 0, glPixelFormat, GL_UNSIGNED_BYTE, nullptr);

    if (buffer.getData())
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, buffer.getWidth(), buffer.getHeight(), glPixelFormat, GL_UNSIGNED_BYTE, buffer.getData());

    if (m_params.isMipmapped())
        glGenerateMipmap(GL_TEXTURE_2D);

    if (m_params.getAnisotropyLevel() != 1)
    {
        float aniso = svc().renderMgr.getRenderer()->getMaxAnisotropy();
        if (m_params.getAnisotropyLevel() != ANISOTROPY_LEVEL_MAX)
        { 
            aniso = (float)m_params.getAnisotropyLevel();
        }

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    printOpenGLError();

    return true;
}

void Texture2D::deleteGLTexture()
{
    if (m_glid)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &m_glid);

        m_glid = 0;
    }
}

Texture2D::Texture2D(TextureCreationParams params)
    : Texture(params)
{

}

Texture2D::Texture2D(const PixelBuffer &pixelBuffer, TextureCreationParams params)
    : Texture(params)
{
    createGLTexture(pixelBuffer);
}

Texture2D::~Texture2D()
{
    deleteGLTexture();
}

size_t Texture2D::getOriginalWidth() const
{
    return m_originalWidth;
}

size_t Texture2D::getOriginalHeight() const
{
    return m_originalHeight;
}

size_t Texture2D::getActualWidth() const
{
    return m_actualWidth;
}

size_t Texture2D::getActualHeight() const
{
    return m_actualHeight;
}

bool Texture2D::bind(size_t unit)
{
    if (!isInitialized())
        return false;

    assert(m_glid);

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_glid);

    return true;
}

void Texture2D::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

}
