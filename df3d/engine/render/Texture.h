#pragma once

#include <df3d/engine/resources/Resource.h>
#include "RenderCommon.h"

namespace df3d {

struct TextureInfo
{
    PixelFormat format = PixelFormat::INVALID;
    size_t width = 0;
    size_t height = 0;
    size_t numMips = 0;
    uint32_t flags = 0;
};

class Texture : public Resource
{
    TextureHandle m_handle;
    size_t m_width;
    size_t m_height;

public:
    Texture(TextureHandle handle = {}, size_t width = 0, size_t height = 0);
    ~Texture();

    // Texture is owning this handle.
    TextureHandle getHandle() const;
    void setHandle(TextureHandle handle);

    size_t getWidth() const { return m_width; }
    size_t getHeight() const { return m_height; }

    void setWidthAndHeight(size_t w, size_t h)
    {
        m_width = w;
        m_height = h;
    }
};

}
