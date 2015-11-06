#include "Texture.h"

#include <base/Service.h>
#include "RendererBackend.h"

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
        glog << "Invalid pixel format. Can't get size." << logwarn;
    }

    return 0;
}

PixelBuffer::PixelBuffer(int w, int h, PixelFormat format)
    : m_w(w), m_h(h), m_format(format)
{

}

PixelBuffer::PixelBuffer(int w, int h, const unsigned char *data, PixelFormat format)
    : m_w(w), m_h(h), m_format(format)
{
    m_dataSize = m_w * m_h * GetPixelSizeForFormat(m_format);

    assert(m_dataSize != 0);

    m_data = new unsigned char[m_dataSize];
    memcpy(m_data, data, m_dataSize);
}

PixelBuffer::~PixelBuffer()
{
    delete [] m_data;
}

TextureCreationParams::TextureCreationParams()
{
    m_filtering = TextureFiltering::TRILINEAR;
    m_mipmapped = true;
    m_wrapMode = TextureWrapMode::WRAP;
    m_anisotropyLevel = ANISOTROPY_LEVEL_MAX;
}

void TextureCreationParams::setFiltering(TextureFiltering filtering)
{
    m_filtering = filtering;
}

void TextureCreationParams::setMipmapped(bool mipmapped)
{
    m_mipmapped = mipmapped;
}

void TextureCreationParams::setAnisotropyLevel(int anisotropy)
{
    float maxSupportedAniso = svc().renderMgr.getRenderer()->getMaxAnisotropy();
    if (anisotropy > maxSupportedAniso)
    {
        glog << "Anisotropy level is bigger than supported. Setting maximum." << logwarn;
        m_anisotropyLevel = (int)maxSupportedAniso;
    }
    else
    {
        m_anisotropyLevel = anisotropy;
    }
}

void TextureCreationParams::setWrapMode(TextureWrapMode wrapMode)
{
    m_wrapMode = wrapMode;
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

GLint Texture::getGlWrapMode(TextureWrapMode mode)
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

void Texture::setupGlTextureFiltering(GLenum glType, TextureFiltering filtering, bool mipmapped)
{
    glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, filtering == TextureFiltering::NEAREST ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, getGlFilteringMode(filtering, mipmapped));

    printOpenGLError();
}

void Texture::setupGlWrapMode(GLenum glType, TextureWrapMode wrapMode)
{
    auto wmGl = getGlWrapMode(wrapMode);
    glTexParameteri(glType, GL_TEXTURE_WRAP_S, wmGl);
    glTexParameteri(glType, GL_TEXTURE_WRAP_T, wmGl);
#if defined(DF3D_DESKTOP)
    glTexParameteri(glType, GL_TEXTURE_WRAP_R, wmGl);
#endif

    printOpenGLError();
}

Texture::Texture(TextureCreationParams params)
    : m_params(params)
{

}

} }
