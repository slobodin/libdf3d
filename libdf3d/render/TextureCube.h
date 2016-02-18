#pragma once

#include "Texture.h"

namespace df3d {

class TextureCubeFSLoader;

class TextureCube : public Texture
{
    friend class TextureCubeFSLoader;

    size_t m_sizeInBytes = 0;

    bool createGLTexture(unique_ptr<PixelBuffer> images[(size_t)CubeFace::COUNT]);
    void deleteGLTexture();

    TextureCube(TextureCreationParams params);

public:
    ~TextureCube();

    bool bind() override;
    void unbind() override;

    size_t getSizeInBytes() const override;
};

}
