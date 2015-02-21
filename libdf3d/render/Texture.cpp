#include "df3d_pch.h"
#include "Texture.h"

#include <base/SystemsMacro.h>
#include "Renderer.h"

namespace df3d { namespace render {

int GetPixelSizeForFormat(PixelFormat format)
{
    switch (format)
    {
    case PixelFormat::RGB:
    case PixelFormat::BGR:
        return 3;
    case PixelFormat::RGBA:
        return 4;
    case PixelFormat::GRAYSCALE:
        return 1;
    case PixelFormat::INVALID:
    default:
        base::glog << "Invalid pixel format. Can't get size." << base::logwarn;
    }

    return 0;
}

bool Texture::isPot(size_t v)
{
    return v && !(v & (v - 1));
}

size_t Texture::getNextPot(size_t v)
{
    if (!isPot(v))
    {
        int n = 0;
        while (v >>= 1)
            ++n;

        v = 1 << (n + 1);
    }
    return v;
}

GLint Texture::getGlFilteringMode(TextureFiltering filtering, bool mipmapped)
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

GLint Texture::getGlWrapMode(Texture::WrapMode mode)
{
    switch (mode)
    {
    case Texture::WrapMode::WRAP:
        return GL_REPEAT;
    case Texture::WrapMode::CLAMP:
        return GL_CLAMP_TO_EDGE;
    default:
        break;
    }

    return -1;
}

void Texture::setupGlTextureFiltering(GLenum glType, TextureFiltering filtering, bool mipmapped)
{
    glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, filtering == TextureFiltering::NEAREST ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, getGlFilteringMode(filtering, mipmapped));

    printOpenGLError();
}

void Texture::setupGlWrapMode(GLenum glType, WrapMode wrapMode)
{
    auto wmGl = getGlWrapMode(wrapMode);
    glTexParameteri(glType, GL_TEXTURE_WRAP_S, wmGl);
    glTexParameteri(glType, GL_TEXTURE_WRAP_T, wmGl);
#if defined(DF3D_WINDOWS)// || defined(DF3D_WINDOWS_PHONE)
    glTexParameteri(glType, GL_TEXTURE_WRAP_R, wmGl);
#endif

    printOpenGLError();
}

TextureFiltering Texture::filtering() const
{
    return m_filtering.get();
}

bool Texture::isMipmapped() const
{
    return m_mipmapped.get();
}

int Texture::anisotropyLevel() const
{
    return m_anisotropyLevel.get();
}

void Texture::setFilteringMode(TextureFiltering newFiltering)
{
    if (m_glid)
        throw std::runtime_error("Failed to set filtering mode. GL texture already created.");

    m_filtering = newFiltering;
}

void Texture::setMipmapped(bool hasMipmaps)
{
    if (m_glid)
        throw std::runtime_error("Failed to set mipmaps. GL texture already created.");

    m_mipmapped = hasMipmaps;
}

void Texture::setWrapMode(WrapMode mode)
{
    if (m_glid)
        throw std::runtime_error("Failed to set wrap mode. GL texture already created.");

    m_wrapMode = mode;
}

void Texture::setMaxAnisotropy(int aniso)
{
    if (m_glid)
        throw std::runtime_error("Failed to set anisotropy level. GL texture already created.");

    float maxSupportedAniso = g_renderManager->getRenderer()->getMaxAnisotropy();
    if (aniso > maxSupportedAniso)
    {
        base::glog << "Anisotropy level is bigger than supported. Setting maximum." << base::logwarn;
        m_anisotropyLevel = (int)maxSupportedAniso;
    }
    else
    {
        m_anisotropyLevel = aniso;
    }
}

} }
