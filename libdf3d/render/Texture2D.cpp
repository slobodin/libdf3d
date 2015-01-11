#include "df3d_pch.h"
#include "Texture2D.h"

#include "Image.h"
#include <base/Controller.h>
#include "RenderManager.h"
#include "Renderer.h"

namespace df3d { namespace render {

bool Texture2D::createGLTexture()
{
    if (m_imageDirty)
    {
        deleteGLTexture();
        m_imageDirty = false;
    }

    if (m_glid)
        return true;

    auto actWidth = /*getNextPot(*/m_image->width();
    auto actHeight = /*getNextPot(*/m_image->height();
    auto maxSize = g_renderManager->getRenderer()->getMaxTextureSize();
    if (actWidth > maxSize || actHeight > maxSize)
    {
        base::glog << "Failed to create texture. Size is too big." << base::logwarn;
        return false;
    }

    m_actualWidth = actWidth;
    m_actualHeight = actHeight;

    if (!(m_actualWidth == m_image->width() && m_actualHeight == m_image->height()))
        base::glog << "Texture with name" << m_image->getGUID() << "is not pot" << base::logdebug;

    glGenTextures(1, &m_glid);
    glBindTexture(GL_TEXTURE_2D, m_glid);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    auto wrapMode = getGlWrapMode(m_wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
#if defined(__WINDOWS__) || defined(__linux__)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapMode);
#endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_filteringMode == Filtering::NEAREST ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getGlFilteringMode(m_filteringMode, m_mipmapped));

    GLint glPixelFormat = 0;
    switch (m_image->pixelFormat())
    {
    case Image::Format::RGB:
    case Image::Format::BGR:
        glPixelFormat = GL_RGB;
        break;
    case Image::Format::RGBA:
        glPixelFormat = GL_RGBA;
        break;
    case Image::Format::GRAYSCALE:
        glPixelFormat = GL_LUMINANCE;   // FIXME: is it valid on ES?
        break;
    default:
        base::glog << "Invalid GL texture pixel format" << base::logwarn;
        return false;
    }

    // FIXME:
    // Init empty texture.
    glTexImage2D(GL_TEXTURE_2D, 0, glPixelFormat, m_actualWidth, m_actualHeight, 0, glPixelFormat, GL_UNSIGNED_BYTE, nullptr);

    if (m_image->data())
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_image->width(), m_image->height(), glPixelFormat, GL_UNSIGNED_BYTE, m_image->data());

    if (m_mipmapped)
        glGenerateMipmap(GL_TEXTURE_2D);

    if (m_maxAnisotropy != 1)
    {
        float aniso = g_renderManager->getRenderer()->getMaxAnisotropy();
        if (m_maxAnisotropy == -1)
        {
            // Use max value.
        }
        else if (m_maxAnisotropy > aniso)
        {
            base::glog << "Anisotropy level is bigger than supported. Setting maximum." << base::logwarn;
        }
        else
        { 
            aniso = m_maxAnisotropy;
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

Texture2D::Texture2D(shared_ptr<Image> image)
{
    setImage(image);
}

Texture2D::~Texture2D()
{
    deleteGLTexture();
}

void Texture2D::setImage(shared_ptr<Image> image)
{
    if (!image)
    {
        base::glog << "Trying to set null image to a texture" << base::logwarn;
        return;
    }

    m_image = image;
    m_imageDirty = true;
}

shared_ptr<const Image> Texture2D::getImage() const
{
    return m_image;
}

size_t Texture2D::getOriginalWidth() const
{
    return m_image ? m_image->width() : 0;
}

size_t Texture2D::getOriginalHeight() const
{
    return m_image ? m_image->height() : 0;
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
    if (!m_image || !m_image->valid())
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