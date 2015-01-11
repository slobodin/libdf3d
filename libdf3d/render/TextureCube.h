#pragma once

#include "Texture.h"

namespace df3d { namespace render {

class Image;

class TextureCube : public Texture
{
    shared_ptr<Image> m_images[6];

    bool imagesValid() const;
    bool createGLTexture();
    void deleteGLTexture();

public:
    TextureCube(shared_ptr<Image> positiveX, shared_ptr<Image> negativeX,
        shared_ptr<Image> positiveY, shared_ptr<Image> negativeY, 
        shared_ptr<Image> positiveZ, shared_ptr<Image> negativeZ);
    ~TextureCube();

    bool bind(size_t unit) override;
    void unbind() override;
};

} }