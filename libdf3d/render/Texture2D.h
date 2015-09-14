#pragma once

#include "Texture.h"

FWD_MODULE_CLASS(resources, Texture2DManualLoader)
FWD_MODULE_CLASS(resources, Texture2DFSLoader)

namespace df3d { namespace render {

class Texture2D : public Texture
{
    friend class resources::Texture2DManualLoader;
    friend class resources::Texture2DFSLoader;

    size_t m_originalWidth = 0, m_originalHeight = 0;
    size_t m_actualWidth = 0, m_actualHeight = 0;

    bool createGLTexture(const PixelBuffer &buffer);
    void deleteGLTexture();

    Texture2D(TextureCreationParams params);
    Texture2D(const PixelBuffer &pixelBuffer, TextureCreationParams params);

public:
    ~Texture2D();

    size_t getOriginalWidth() const;
    size_t getOriginalHeight() const;
    size_t getActualWidth() const;
    size_t getActualHeight() const;

    bool bind(size_t unit) override;
    void unbind() override;
};

} }
