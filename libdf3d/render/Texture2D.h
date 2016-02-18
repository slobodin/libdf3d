#pragma once

#include "Texture.h"

namespace df3d {

class Texture2DManualLoader;
class Texture2DFSLoader;

class Texture2D : public Texture
{
    friend class Texture2DManualLoader;
    friend class Texture2DFSLoader;

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

    bool bind() override;
    void unbind() override;
};

}
