#include "df3d_pch.h"
#include "Texture2D.h"

#include <base/SystemsMacro.h>
#include "Renderer.h"

namespace df3d { namespace render {

bool Texture2D::createGLTexture()
{
    if (!m_pixelBuffer)
        return false;

    if (m_pixelBufferDirty)
    {
        deleteGLTexture();
        m_pixelBufferDirty = false;
    }

    if (m_glid)
        return true;

    auto actWidth = /*getNextPot(*/m_pixelBuffer->w;
    auto actHeight = /*getNextPot(*/m_pixelBuffer->h;
    auto maxSize = g_renderManager->getRenderer()->getMaxTextureSize();
    if (actWidth > maxSize || actHeight > maxSize)
    {
        base::glog << "Failed to create texture. Size is too big." << base::logwarn;
        return false;
    }

    m_actualWidth = actWidth;
    m_actualHeight = actHeight;

    if (!(m_actualWidth == m_pixelBuffer->w && m_actualHeight == m_pixelBuffer->h))
        base::glog << "Texture with name" << getGUID() << "is not pot" << base::logdebug;

    glGenTextures(1, &m_glid);
    glBindTexture(GL_TEXTURE_2D, m_glid);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    const auto &defaultCaps = g_renderManager->getRenderingCapabilities();
    if (!m_filtering)
        m_filtering = defaultCaps.textureFiltering;
    if (!m_mipmapped)
        m_mipmapped = defaultCaps.mipmaps;

    setupGlWrapMode(GL_TEXTURE_2D, m_wrapMode);
    setupGlTextureFiltering(GL_TEXTURE_2D, filtering(), isMipmapped());

    GLint glPixelFormat = 0;
    switch (m_pixelBuffer->format)
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
        base::glog << "Invalid GL texture pixel format" << base::logwarn;
        return false;
    }

    // FIXME:
    // Init empty texture.
    glTexImage2D(GL_TEXTURE_2D, 0, glPixelFormat, m_actualWidth, m_actualHeight, 0, glPixelFormat, GL_UNSIGNED_BYTE, nullptr);

    if (m_pixelBuffer->data) 
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_pixelBuffer->w, m_pixelBuffer->h, glPixelFormat, GL_UNSIGNED_BYTE, m_pixelBuffer->data);

        delete [] m_pixelBuffer->data;
        m_pixelBuffer->data = nullptr;
    }

    if (isMipmapped())
        glGenerateMipmap(GL_TEXTURE_2D);

    if (!m_anisotropyLevel)
        m_anisotropyLevel = defaultCaps.anisotropyMax;

    if (*m_anisotropyLevel != 1)
    {
        float aniso = g_renderManager->getRenderer()->getMaxAnisotropy();
        if (*m_anisotropyLevel != ANISOTROPY_LEVEL_MAX)
        { 
            aniso = (float)*m_anisotropyLevel;
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

Texture2D::Texture2D()
{

}

Texture2D::~Texture2D()
{
    deleteGLTexture();
}

void Texture2D::setEmpty(size_t width, size_t height, PixelFormat format)
{
    m_pixelBuffer = make_unique<PixelBuffer>();
    m_pixelBuffer->w = width;
    m_pixelBuffer->h = height;
    m_pixelBuffer->format = format;
    m_pixelBufferDirty = true;

    setInitialized();
}

void Texture2D::setWithData(size_t width, size_t height, PixelFormat format, const unsigned char *data)
{
    auto dataSize = width * height * GetPixelSizeForFormat(format);

    m_pixelBuffer = make_unique<PixelBuffer>();
    m_pixelBuffer->w = width;
    m_pixelBuffer->h = height;
    m_pixelBuffer->format = format;
    m_pixelBuffer->data = new unsigned char[dataSize];
    memcpy(m_pixelBuffer->data, data, dataSize);
    m_pixelBufferDirty = true;

    setInitialized();
}

const unsigned char *Texture2D::getPixelBufferData() const
{
    return m_pixelBuffer->data;
}

PixelFormat Texture2D::getPixelFormat() const
{
    return m_pixelBuffer->format;
}

size_t Texture2D::getOriginalWidth() const
{
    return m_pixelBuffer->w;
}

size_t Texture2D::getOriginalHeight() const
{
    return m_pixelBuffer->h;
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
    if (!valid())
        return false;

    if (createGLTexture())
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_glid);
        return true;
    }
    else
        return false;
}

void Texture2D::unbind()
{
    if (!m_glid)
        return;

    glBindTexture(GL_TEXTURE_2D, 0);
}

} }
