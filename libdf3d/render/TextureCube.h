#pragma once

#include "Texture.h"

namespace df3d { namespace render {

class Texture2D;

class TextureCube : public Texture
{
    shared_ptr<Texture2D> m_images[6];

    bool imagesValid() const;
    bool createGLTexture();
    void deleteGLTexture();

public:
    TextureCube(shared_ptr<Texture2D> positiveX, shared_ptr<Texture2D> negativeX,
                shared_ptr<Texture2D> positiveY, shared_ptr<Texture2D> negativeY,
                shared_ptr<Texture2D> positiveZ, shared_ptr<Texture2D> negativeZ);
    ~TextureCube();

    bool bind(size_t unit) override;
    void unbind() override;
};

} }
