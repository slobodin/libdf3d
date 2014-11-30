#pragma once

#include <resources/Resource.h>

namespace df3d { namespace render {

class Image : public resources::Resource
{
public:
    //! Each channel is 8 bits.
    enum class Format
    {
        INVALID,
        RGB,
        BGR,
        RGBA,
        GRAYSCALE
    };

private:
    //! Pixel data of a texture.
    unsigned char *m_data = nullptr;
    size_t m_width = 0;
    size_t m_height = 0;
    //! Bytes per pixel.
    size_t m_depth = 0;

    Format m_pixelFormat = Format::RGB;

public:
    Image();
    ~Image();

    bool init();

    //! Image width.
    size_t width() const;
    //! Image height.
    size_t height() const;
    //! Bytes per pixel.
    size_t depth() const;
    //! Get the raw pixel data.
    const unsigned char *data() const;
    //! Returns pixel at given coordinates.
    const unsigned char *pixelAt(size_t x, size_t y) const;
    //! Returns image format.
    Format pixelFormat() const;

    void setWidth(size_t w);
    void setHeight(size_t h);
    void setPixelFormat(Format pf);

    //! Initializes image with given pixel array.
    void setWithData(const unsigned char *data, size_t w, size_t h, Format pf, int pitch = -1);
};

} }