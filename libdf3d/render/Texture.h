#pragma once

#include <resources/Resource.h>
#include "OpenGLCommon.h"
#include "RenderCommon.h"

namespace df3d { namespace render {

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

class Texture : public resources::Resource
{
protected:
    TextureCreationParams m_params;

    GLuint m_glid = 0;

    // Helpers.
    static bool isPot(size_t v);
    static size_t getNextPot(size_t v);
    static GLint getGlFilteringMode(TextureFiltering filtering, bool mipmapped);
    static GLint getGlWrapMode(TextureWrapMode mode);

    static void setupGlTextureFiltering(GLenum glType, TextureFiltering filtering, bool mipmapped);
    static void setupGlWrapMode(GLenum glType, TextureWrapMode wrapMode);

public:
    Texture(TextureCreationParams params);

    unsigned getGLId() const { return m_glid; }
    const TextureCreationParams& getParams() const { return m_params; }

    virtual bool bind(size_t unit) = 0;
    virtual void unbind() = 0;
};

} }
