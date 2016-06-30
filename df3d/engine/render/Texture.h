#pragma once

#include <df3d/engine/resources/Resource.h>
#include "RenderCommon.h"

namespace df3d {

class PixelBuffer : utils::NonCopyable
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
    bool m_mipmapped;
    int m_anisotropyLevel;
    TextureWrapMode m_wrapMode;

public:
    TextureCreationParams();

    TextureFiltering getFiltering() const { return m_filtering; }
    bool isMipmapped() const { return m_mipmapped; }
    int getAnisotropyLevel() const { return m_anisotropyLevel; }
    TextureWrapMode getWrapMode() const { return m_wrapMode; }

    void setFiltering(TextureFiltering filtering);
    void setMipmapped(bool mipmapped);
    void setAnisotropyLevel(int anisotropy);
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
    TextureDescriptor m_descr;
    TextureInfo m_info;

public:
    Texture(TextureDescriptor descr = {}, const TextureInfo &info = {});
    ~Texture();

    // Texture is owning this descriptor.
    TextureDescriptor getDescriptor() const;
    void setDescriptor(TextureDescriptor descr);

    void setTextureInfo(const TextureInfo &info) { m_info = info; }
    const TextureInfo& getTextureInfo() const { return m_info; }
};

int GetPixelSizeForFormat(PixelFormat format);

}
