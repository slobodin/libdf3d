#pragma once

#include "Texture.h"

namespace df3d { namespace render {

class Texture2D : public Texture
{
    friend class Texture2DManualLoader;

    size_t m_originalWidth = 0, m_originalHeight = 0;
    size_t m_actualWidth = 0, m_actualHeight = 0;

    bool createGLTexture(const PixelBuffer &buffer);
    void deleteGLTexture();

    Texture2D() = default;
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

class Texture2DManualLoader : public resources::ManualResourceLoader
{
    unique_ptr<PixelBuffer> m_pixelBuffer;
    TextureCreationParams m_params;

public:
    Texture2DManualLoader(unique_ptr<PixelBuffer> pixelBuffer, TextureCreationParams params);

    Texture2D* load() override;
};

class Texture2DFSLoader : public resources::FSResourceLoader
{
public:
};

} }
