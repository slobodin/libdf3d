#include "Texture.h"

#include <df3d/engine/EngineController.h>
#include "RenderManager.h"
#include "IRenderBackend.h"

namespace df3d {

int GetPixelSizeForFormat(PixelFormat format)
{
    switch (format)
    {
    case PixelFormat::RGB:
        return 3;
    case PixelFormat::RGBA:
        return 4;
    case PixelFormat::INVALID:
    default:
        DFLOG_WARN("Invalid pixel format. Can't get size.");
    }

    return 0;
}

PixelBuffer::PixelBuffer(int w, int h, uint8_t *data, PixelFormat format, bool copyData)
    :  m_format(format), m_w(w), m_h(h)
{
    m_dataSize = m_w * m_h * GetPixelSizeForFormat(m_format);
    DF3D_ASSERT(m_dataSize != 0);

    if (copyData)
    {
        m_data = new uint8_t[m_dataSize];
        memcpy(m_data, data, m_dataSize);
    }
    else
        m_data = data;
}

PixelBuffer::~PixelBuffer()
{
    delete [] m_data;
}

TextureCreationParams::TextureCreationParams()
{
    m_filtering = svc().renderManager().getRenderingCapabilities().getFiltering();
    m_mipmapped = svc().renderManager().getRenderingCapabilities().hasMipmaps();
    m_wrapMode = TextureWrapMode::WRAP;
    m_maxAnisotropy = true;     // FIXME: move to params + refactor.
}

void TextureCreationParams::setFiltering(TextureFiltering filtering)
{
    m_filtering = filtering;
}

void TextureCreationParams::setMipmapped(bool mipmapped)
{
    m_mipmapped = mipmapped;
}

void TextureCreationParams::setWrapMode(TextureWrapMode wrapMode)
{
    m_wrapMode = wrapMode;
}

Texture::Texture(TextureHandle handle, const TextureInfo &info)
    : m_handle(handle),
    m_info(info)
{

}

Texture::~Texture()
{
    if (m_handle.valid())
        svc().renderManager().getBackend().destroyTexture(m_handle);
}

TextureHandle Texture::getHandle() const
{
    return m_handle;
}

void Texture::setHandle(TextureHandle handle)
{
    DF3D_ASSERT(handle.valid());

    if (m_handle.valid())
    {
        DFLOG_WARN("Texture already has a handle");
        return;
    }

    m_handle = handle;
}

}
