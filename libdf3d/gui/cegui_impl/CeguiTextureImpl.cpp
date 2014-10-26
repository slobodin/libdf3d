#include "df3d_pch.h"
#include "CeguiTextureImpl.h"

#include <render/Image.h>
#include <render/Texture.h>

namespace df3d { namespace gui { namespace cegui_impl {

CeguiTextureImpl::CeguiTextureImpl(const CEGUI::String &name)
    : m_name(name)
{
    // generate texture
}

CeguiTextureImpl::CeguiTextureImpl(const CEGUI::String &name, const CEGUI::String &filename, const CEGUI::String &resourceGroup)
    : CeguiTextureImpl(name)
{
    loadFromFile(filename, resourceGroup);
}

CeguiTextureImpl::CeguiTextureImpl(const CEGUI::String &name, const CEGUI::Sizef &size)
    : CeguiTextureImpl(name)
{
    m_originalDataSize = size;

    auto textureImage = make_shared<render::Image>();
    textureImage->setWidth(size.d_width);
    textureImage->setHeight(size.d_height);
    textureImage->setPixelFormat(render::Image::PF_RGBA);
    textureImage->setInitialized();

    auto texture = make_shared<render::Texture>();
    texture->setImage(textureImage);

    texture->setType(render::Texture::TEXTURE_2D);
    texture->setFilteringMode(render::Texture::BILINEAR);
    texture->setMipmapped(false);

    //m_dataSize.d_width = texture->getActualWidth();
    //m_dataSize.d_height = texture->getActualHeight();

    // TODO:
    // create empty texture of size 'size'.
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

}

void CeguiTextureImpl::loadFromMemory(const void *buffer, const CEGUI::Sizef &buffer_size, PixelFormat pixel_format)
{

}

void CeguiTextureImpl::blitFromMemory(const void *sourceData, const CEGUI::Rectf &area)
{

}

void CeguiTextureImpl::blitToMemory(void *targetData)
{

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