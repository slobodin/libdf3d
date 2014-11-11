#include "df3d_pch.h"
#include "Image.h"

namespace df3d { namespace render {

Image::Image()
{

}

Image::~Image()
{
    SAFE_ARRAY_DELETE(m_data);
}

bool Image::init()
{
    return (m_data != nullptr) && (m_width > 0) && (m_height > 0) && (m_depth > 0);
}

size_t Image::width() const
{
    if (!valid())
        return 0;
    return m_width;
}

size_t Image::height() const
{
    if (!valid())
        return 0;
    return m_height;
}

size_t Image::depth() const
{
    if (!valid())
        return 0;
    return m_depth;
}

const unsigned char *Image::data() const
{
    if (!valid())
        return nullptr;
    return m_data;
}

const unsigned char *Image::pixelAt(size_t x, size_t y) const
{
    if (!valid())
        return nullptr;
    return m_data + m_width * y * m_depth + x * m_depth;
}

Image::Format Image::pixelFormat() const
{
    if (!valid())
        return Format::INVALID;
    return m_pixelFormat;
}

void Image::setWidth(size_t w)
{
    m_width = w;
}

void Image::setHeight(size_t h)
{
    m_height = h;
}

void Image::setPixelFormat(Format pf)
{
    switch (pf)
    {
    case Format::RGB:
    case Format::BGR:
        m_depth = 3;
        break;
    case Format::RGBA:
        m_depth = 4;
        break;
    case Format::GRAYSCALE:
        m_depth = 1;
        break;
    case Format::INVALID:
    default:
        base::glog << "Trying to set up invalid pixel format for image" << getGUID() << base::logwarn;
        return;
    }

    m_pixelFormat = pf;
}

void Image::setWithData(const unsigned char *data, size_t w, size_t h, Format pf, int pitch)
{
    if (!data || pf == Format::INVALID)
        return;

    setPixelFormat(pf);
    m_width = w;
    m_height = h;

    auto size = w * h * m_depth;
    if (size == 0)
        return;

    if (pitch == -1)
        pitch = w * m_depth;

    SAFE_ARRAY_DELETE(m_data);

    m_data = new unsigned char[size];

    for (size_t ln = 0; ln < m_height; ln++)
    {
        const unsigned char *src = data + pitch * ln;
        unsigned char *dst = m_data + ln * m_width * m_depth;

        memcpy(dst, src, m_width * m_depth);
    }
}

void Image::setWithData(SDL_Surface *surf)
{
    // FIXME:
    // Format
    auto pf = Format::INVALID;
    if (surf->format->BytesPerPixel == 1)
        pf = Format::GRAYSCALE;
    else if (surf->format->BytesPerPixel == 3)
        pf = Format::RGB;
    else if (surf->format->BytesPerPixel == 4)
        pf = Format::RGBA;

    SDL_LockSurface(surf);
    setWithData((const unsigned char *)surf->pixels, surf->w, surf->h, pf, surf->pitch);
    SDL_UnlockSurface(surf);
}

} }