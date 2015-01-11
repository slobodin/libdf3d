#include "df3d_pch.h"
#include "Texture.h"

namespace df3d { namespace render {

const int ANISOTROPY_LEVEL_MAX = -1;

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

GLint Texture::getGlFilteringMode(Texture::Filtering filtering, bool mipmapped)
{
    switch (filtering)
    {
    case Texture::Filtering::NEAREST:
        return !mipmapped ? GL_NEAREST : GL_NEAREST_MIPMAP_NEAREST;
    case Texture::Filtering::BILINEAR:
        return !mipmapped ? GL_LINEAR : GL_LINEAR_MIPMAP_NEAREST;
    case Texture::Filtering::TRILINEAR:
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

Texture::Texture()
{

}

Texture::~Texture()
{

}

void Texture::setFilteringMode(Texture::Filtering newFiltering)
{
    m_filteringMode = newFiltering;

    // TODO:
    // Should recreate texture!!!
}

void Texture::setMipmapped(bool hasMipmaps)
{
    m_mipmapped = hasMipmaps;

    // TODO:
    // Should recreate texture!!!
}

void Texture::setWrapMode(WrapMode mode)
{
    m_wrapMode = mode;
}

void Texture::setMaxAnisotropy(int aniso)
{
    m_maxAnisotropy = aniso;
}

} }