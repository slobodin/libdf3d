#pragma once

#include <df3d/engine/resources/Resource.h>
#include "RenderCommon.h"

namespace df3d {

class PixelBuffer : NonCopyable
{
    PixelFormat m_format = PixelFormat::INVALID;
    int m_w = 0;
    int m_h = 0;
    uint8_t *m_data = nullptr;
    size_t m_dataSize = 0;

public:
    PixelBuffer(int w, int h, uint8_t *data, PixelFormat format, bool copyData = true);
    ~PixelBuffer();

    int getWidth() const { return m_w; }
    int getHeight() const { return m_h; }
    PixelFormat getFormat() const { return m_format; }
    const uint8_t* getData() const { return m_data; }
    size_t getSizeInBytes() const { return m_dataSize; }
};

class TextureCreationParams
{
    TextureFiltering m_filtering;
    TextureWrapMode m_wrapMode;
    bool m_mipmapped;
    bool m_maxAnisotropy;

public:
    TextureCreationParams();

    TextureFiltering getFiltering() const { return m_filtering; }
    bool isMipmapped() const { return m_mipmapped; }
    bool isAnisotropyMax() const { return m_maxAnisotropy; }
    TextureWrapMode getWrapMode() const { return m_wrapMode; }

    void setFiltering(TextureFiltering filtering);
    void setMipmapped(bool mipmapped);
    void setAnisotropyMax(bool isMax) { m_maxAnisotropy = isMax; }
    void setWrapMode(TextureWrapMode wrapMode);
};

struct TextureInfo
{
    size_t width = 0;
    size_t height = 0;
    bool isCubemap = false;
    size_t sizeInBytes = 0;
};

class Texture : public Resource
{
    TextureHandle m_handle;
    TextureInfo m_info;

public:
    Texture(TextureHandle handle = {}, const TextureInfo &info = {});
    ~Texture();

    // Texture is owning this handle.
    TextureHandle getHandle() const;
    void setHandle(TextureHandle handle);

    void setTextureInfo(const TextureInfo &info) { m_info = info; }
    const TextureInfo& getTextureInfo() const { return m_info; }
};

int GetPixelSizeForFormat(PixelFormat format);

}
