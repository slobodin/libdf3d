#pragma once

#include "Texture.h"

namespace df3d { namespace render {

class Image;

class Texture2D : public Texture
{
    shared_ptr<Image> m_image;
    bool m_imageDirty = true;
    size_t m_actualWidth = 0, m_actualHeight = 0;

    bool createGLTexture();
    void deleteGLTexture();

public:
    Texture2D();
    Texture2D(shared_ptr<Image> image);
    ~Texture2D();

    void setImage(shared_ptr<Image> image);
    shared_ptr<const Image> getImage() const;

    size_t getOriginalWidth() const;
    size_t getOriginalHeight() const;
    size_t getActualWidth() const;
    size_t getActualHeight() const;

    bool bind(size_t unit) override;
    void unbind() override;
};

} }