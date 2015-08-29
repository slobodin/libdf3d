#pragma once

#include "Texture.h"

FWD_MODULE_CLASS(resources, DecoderTexture)

namespace df3d { namespace render {

class Texture2D : public Texture
{
    friend class resources::DecoderTexture;

    unique_ptr<PixelBuffer> m_pixelBuffer;

    size_t m_actualWidth = 0, m_actualHeight = 0;

    bool createGLTexture();
    void deleteGLTexture();

    void onDecoded(bool decodeResult) override;

    Texture2D() = default;

public:
    Texture2D(unique_ptr<PixelBuffer> pixelBuffer, TextureCreationParams params);
    ~Texture2D();

    size_t getOriginalWidth() const;
    size_t getOriginalHeight() const;
    size_t getActualWidth() const;
    size_t getActualHeight() const;

    bool bind(size_t unit) override;
    void unbind() override;
};

} }
