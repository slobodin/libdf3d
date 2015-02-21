#pragma once

#include "Texture.h"

namespace df3d { namespace render {

class Texture2D : public Texture
{
    unique_ptr<PixelBuffer> m_pixelBuffer;
    bool m_pixelBufferDirty = false;

    size_t m_actualWidth = 0, m_actualHeight = 0;

    bool createGLTexture();
    void deleteGLTexture();

public:
    Texture2D();
    ~Texture2D();

    void setEmpty(size_t width, size_t height, PixelFormat format);
    void setWithData(size_t width, size_t height, PixelFormat format, const unsigned char *data);

    const unsigned char *getPixelBufferData() const;
    PixelFormat getPixelFormat() const;
    size_t getOriginalWidth() const;
    size_t getOriginalHeight() const;
    size_t getActualWidth() const;
    size_t getActualHeight() const;

    bool bind(size_t unit) override;
    void unbind() override;
};

} }
