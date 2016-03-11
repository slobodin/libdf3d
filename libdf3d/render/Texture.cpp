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
    case PixelFormat::BGR:
        return 3;
    case PixelFormat::RGBA:
        return 4;
    case PixelFormat::GRAYSCALE:
        return 1;
    case PixelFormat::INVALID:
    default:
        glog << "Invalid pixel format. Can't get size." << logwarn;
    }

    return 0;
}

PixelBuffer::PixelBuffer(int w, int h, PixelFormat format)
    : m_w(w), m_h(h), m_format(format)
{

}

PixelBuffer::PixelBuffer(int w, int h, const unsigned char *data, PixelFormat format)
    : m_w(w), m_h(h), m_format(format)
{
    m_dataSize = m_w * m_h * GetPixelSizeForFormat(m_format);

    assert(m_dataSize != 0);

    m_data = new unsigned char[m_dataSize];
    memcpy(m_data, data, m_dataSize);
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
        glog << "Anisotropy level is bigger than supported. Setting maximum." << logwarn;
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
    assert(descr.valid());

    if (m_descr.valid())
    {
        glog << "Texture already has a descriptor" << logwarn;
        return;
    }

    m_descr = descr;
}

}
