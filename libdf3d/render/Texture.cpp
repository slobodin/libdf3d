#include "Texture.h"

#include <libdf3d/base/EngineController.h>
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
    DF3D_ASSERT(m_dataSize != 0, "sanity check");

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
    m_anisotropyLevel = svc().renderManager().getRenderingCapabilities().getAnisotropy();
}

void TextureCreationParams::setFiltering(TextureFiltering filtering)
{
    m_filtering = filtering;
}

void TextureCreationParams::setMipmapped(bool mipmapped)
{
    m_mipmapped = mipmapped;
}

void TextureCreationParams::setAnisotropyLevel(int anisotropy)
{
    float maxSupportedAniso = svc().renderManager().getBackend().getCaps().maxAnisotropy;
    if (anisotropy > maxSupportedAniso)
    {
        DFLOG_WARN("Anisotropy level is bigger than supported. Setting maximum.");
        m_anisotropyLevel = (int)maxSupportedAniso;
    }
    else
    {
        m_anisotropyLevel = anisotropy;
    }
}

void TextureCreationParams::setWrapMode(TextureWrapMode wrapMode)
{
    m_wrapMode = wrapMode;
}

Texture::Texture(TextureDescriptor descr, const TextureInfo &info)
    : m_descr(descr),
    m_info(info)
{

}

Texture::~Texture()
{
    if (m_descr.valid())
        svc().renderManager().getBackend().destroyTexture(m_descr);
}

TextureDescriptor Texture::getDescriptor() const
{
    return m_descr;
}

void Texture::setDescriptor(TextureDescriptor descr)
{
    DF3D_ASSERT(descr.valid(), "sanity check");

    if (m_descr.valid())
    {
        DFLOG_WARN("Texture already has a descriptor");
        return;
    }

    m_descr = descr;
}

}
