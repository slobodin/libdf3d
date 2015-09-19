#pragma once

#include "Texture.h"

FWD_MODULE_CLASS(resources, TextureCubeFSLoader)

namespace df3d { namespace render {

class TextureCube : public Texture
{
    friend class resources::TextureCubeFSLoader;

    bool createGLTexture(unique_ptr<PixelBuffer> images[CUBE_FACES_COUNT]);
    void deleteGLTexture();

    TextureCube();

public:
    ~TextureCube();

    bool bind(size_t unit) override;
    void unbind() override;
};

} }
