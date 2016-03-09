#pragma once

#include <libdf3d/resources/Resource.h>
#include "RenderCommon.h"

namespace df3d {

class PixelBuffer : utils::NonCopyable
{
    PixelFormat m_format = PixelFormat::INVALID;
    int m_w = 0;
    int m_h = 0;
    unsigned char *m_data = nullptr;
    size_t m_dataSize = 0;

public:
    PixelBuffer(int w, int h, PixelFormat format);
    PixelBuffer(int w, int h, const unsigned char *data, PixelFormat format);
    ~PixelBuffer();

    int getWidth() const { return m_w; }
    int getHeight() const { return m_h; }
    PixelFormat getFormat() const { return m_format; }
    const unsigned char* getData() const { return m_data; }
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

class Texture : public Resource
{
    TextureDescriptor m_descr;

public:
    Texture(TextureDescriptor descr = {});
    ~Texture();

    // Texture is owning this descriptor.
    TextureDescriptor getDescriptor() const;
    void setDescriptor(TextureDescriptor descr);

    size_t getWidth() const;
    size_t getHeight() const;
};

}
