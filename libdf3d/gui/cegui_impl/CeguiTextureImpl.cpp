#include "df3d_pch.h"
#include "CeguiTextureImpl.h"

#include "CeguiResourceProviderImpl.h"
#include <CEGUI/CEGUI.h>
#include <base/TypeDefs.h>
#include <base/SystemsMacro.h>
#include <render/Texture2D.h>

namespace df3d { namespace gui { namespace cegui_impl {

using namespace CEGUI;

PixelFormat convertPixelFormat(const Texture::PixelFormat fmt)
{
    switch (fmt)
    {
    case Texture::PF_RGBA:
        return PixelFormat::RGBA;
    case Texture::PF_RGB:
        return PixelFormat::RGB;
    case Texture::PF_RGB_565:
    case Texture::PF_RGBA_4444:
    case Texture::PF_PVRTC2:
    case Texture::PF_PVRTC4:
    case Texture::PF_RGBA_DXT1:
    case Texture::PF_RGBA_DXT3:
    case Texture::PF_RGBA_DXT5:
    default:
        break;
    }

    return PixelFormat::INVALID;
}

void CeguiTextureImpl::updateSizes()
{
    m_originalDataSize.d_width = (float)m_texture->getOriginalWidth();
    m_originalDataSize.d_height = (float)m_texture->getOriginalHeight();

    // TODO:
    // Get actual size!
    m_dataSize.d_width = /*getNextPot(*/m_originalDataSize.d_width/*)*/;
    m_dataSize.d_height = /*getNextPot(*/m_originalDataSize.d_height/*)*/;

    updateCachedScaleValues();
}

void CeguiTextureImpl::updateCachedScaleValues()
{
    // calculate what to use for x scale
    const float orgW = m_originalDataSize.d_width;
    const float texW = m_dataSize.d_width;

    // if texture and original data width are the same, scale is based
    // on the original size.
    // if texture is wider (and source data was not stretched), scale
    // is based on the size of the resulting texture.
    if (orgW == texW && orgW == 0.0f)
        m_texelScaling.d_x = 0.0f;
    else
        m_texelScaling.d_x = 1.0f / ((orgW == texW) ? orgW : texW);

    // calculate what to use for y scale
    const float orgH = m_originalDataSize.d_height;
    const float texH = m_dataSize.d_height;

    // if texture and original data height are the same, scale is based
    // on the original size.
    // if texture is taller (and source data was not stretched), scale
    // is based on the size of the resulting texture.
    if (orgH == texH && orgH == 0.0f)
        m_texelScaling.d_x = 0.0f;
    else
        m_texelScaling.d_y = 1.0f / ((orgH == texH) ? orgH : texH);
}

CeguiTextureImpl::CeguiTextureImpl(const CEGUI::String &name)
    : m_name(name)
{
    m_texture = g_resourceManager->createEmptyTexture(m_name.c_str());
    m_texture->setFilteringMode(render::TextureFiltering::BILINEAR);
    m_texture->setMipmapped(false);

    updateSizes();
}

CeguiTextureImpl::CeguiTextureImpl(const CEGUI::String &name, const CEGUI::String &filename, const CEGUI::String &resourceGroup)
    : m_name(name)
{
    loadFromFile(filename, resourceGroup);
}

CeguiTextureImpl::CeguiTextureImpl(const CEGUI::String &name, const CEGUI::Sizef &size)
    : m_name(name)
{
    m_texture = g_resourceManager->createEmptyTexture(m_name.c_str());
    m_texture->setEmpty((size_t)size.d_width, (size_t)size.d_height, df3d::PixelFormat::RGBA);
    m_texture->setFilteringMode(render::TextureFiltering::BILINEAR);
    m_texture->setMipmapped(false);
    
    updateSizes();
}

CeguiTextureImpl::CeguiTextureImpl(const CEGUI::String &name, shared_ptr<render::Texture2D> texture)
    : m_name(name),
    m_texture(texture)
{
    updateSizes();
}

const CEGUI::String& CeguiTextureImpl::getName() const
{
    return m_name;
}

const CEGUI::Sizef& CeguiTextureImpl::getSize() const
{
    return m_dataSize;
}

const CEGUI::Sizef& CeguiTextureImpl::getOriginalDataSize() const
{
    return m_originalDataSize;
}

const CEGUI::Vector2f& CeguiTextureImpl::getTexelScaling() const
{
    return m_texelScaling;
}

void CeguiTextureImpl::loadFromFile(const CEGUI::String &filename, const CEGUI::String &resourceGroup)
{
    auto rp = static_cast<CeguiResourceProviderImpl*>(CEGUI::System::getSingleton().getResourceProvider());
    auto path = rp->getFinalFilename(filename, resourceGroup);
        
    m_texture = g_resourceManager->createTexture(path.c_str(), ResourceLoadingMode::IMMEDIATE);
    m_texture->setFilteringMode(render::TextureFiltering::BILINEAR);
    m_texture->setMipmapped(false);

    updateSizes();
}

void CeguiTextureImpl::loadFromMemory(const void *buffer, const CEGUI::Sizef &buffer_size, PixelFormat pixel_format)
{
    if (!isPixelFormatSupported(pixel_format))
        CEGUI_THROW(InvalidRequestException("Data was supplied in an unsupported pixel format."));

    m_texture = g_resourceManager->createEmptyTexture(m_name.c_str());
    m_texture->setWithData((size_t)buffer_size.d_width, (size_t)buffer_size.d_height, convertPixelFormat(pixel_format), (const unsigned char *)buffer);

    updateSizes();
}

void CeguiTextureImpl::blitFromMemory(const void *sourceData, const CEGUI::Rectf &area)
{
    CEGUI_THROW(InvalidRequestException("CeguiTextureImpl::blitFromMemory is not supported."));
}

void CeguiTextureImpl::blitToMemory(void *targetData)
{
    CEGUI_THROW(InvalidRequestException("CeguiTextureImpl::blitToMemory is not supported."));
}

bool CeguiTextureImpl::isPixelFormatSupported(const PixelFormat fmt) const
{
    switch (fmt)
    {
    case PF_RGB:
        return true;
    case PF_RGBA:
        return true;
    case PF_RGBA_4444:
    case PF_RGB_565:
    case PF_PVRTC2:
    case PF_PVRTC4:
    case PF_RGB_DXT1:
    case PF_RGBA_DXT1:
    case PF_RGBA_DXT3:
    case PF_RGBA_DXT5:
    default:
        break;
    }

    return false;
}

shared_ptr<render::Texture> CeguiTextureImpl::getDf3dTexture() const
{
    return m_texture;
}

} } }
