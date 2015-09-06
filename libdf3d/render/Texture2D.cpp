#include "df3d_pch.h"
#include "Texture2D.h"

#include <base/SystemsMacro.h>
#include "RendererBackend.h"

namespace df3d { namespace render {

bool Texture2D::createGLTexture()
{
    auto actWidth = /*getNextPot(*/m_pixelBuffer->getWidth();
    auto actHeight = /*getNextPot(*/m_pixelBuffer->getHeight();
    auto maxSize = g_renderManager->getRenderer()->getMaxTextureSize();
    if (actWidth > maxSize || actHeight > maxSize)
    {
        base::glog << "Failed to create texture. Size is too big." << base::logwarn;
        return false;
    }

    GLint glPixelFormat = 0;
    switch (m_pixelBuffer->getFormat())
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

    m_actualWidth = actWidth;
    m_actualHeight = actHeight;

    if (!(m_actualWidth == m_pixelBuffer->getWidth() && m_actualHeight == m_pixelBuffer->getHeight()))
        base::glog << "Texture with name" << getGUID() << "is not pot" << base::logdebug;

    glGenTextures(1, &m_glid);
    glBindTexture(GL_TEXTURE_2D, m_glid);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    setupGlWrapMode(GL_TEXTURE_2D, m_params.getWrapMode());
    setupGlTextureFiltering(GL_TEXTURE_2D, m_params.getFiltering(), m_params.isMipmapped());

    // FIXME:
    // Init empty texture.
    glTexImage2D(GL_TEXTURE_2D, 0, glPixelFormat, m_actualWidth, m_actualHeight, 0, glPixelFormat, GL_UNSIGNED_BYTE, nullptr);

    if (m_pixelBuffer->getData())
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_pixelBuffer->getWidth(), m_pixelBuffer->getHeight(), glPixelFormat, GL_UNSIGNED_BYTE, m_pixelBuffer->getData());

        m_pixelBuffer.reset();
    }

    if (m_params.isMipmapped())
        glGenerateMipmap(GL_TEXTURE_2D);

    if (m_params.getAnisotropyLevel() != 1)
    {
        float aniso = g_renderManager->getRenderer()->getMaxAnisotropy();
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

void Texture2D::onDecoded(bool decodeResult)
{
    if (!decodeResult)
    {
        base::glog << "Texture2D::onDecoded failed" << base::logwarn;
        return;
    }

    m_initialized = createGLTexture();
}

Texture2D::Texture2D(unique_ptr<PixelBuffer> pixelBuffer, TextureCreationParams params)
    : Texture(params),
    m_pixelBuffer(std::move(pixelBuffer))
{
    onDecoded(true);
}

Texture2D::~Texture2D()
{
    deleteGLTexture();
}

size_t Texture2D::getOriginalWidth() const
{
    if (!isInitialized())
        return 0;
    return m_pixelBuffer->getWidth();
}

size_t Texture2D::getOriginalHeight() const
{
    if (!isInitialized())
        return 0;
    return m_pixelBuffer->getHeight();
}

size_t Texture2D::getActualWidth() const
{
    if (!isInitialized())
        return 0;
    return m_actualWidth;
}

size_t Texture2D::getActualHeight() const
{
    if (!isInitialized())
        return 0;
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

} }
