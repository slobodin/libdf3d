#include "df3d_pch.h"
#include "CeguiTextureImpl.h"

namespace df3d { namespace gui { namespace cegui_impl {

CeguiTextureImpl::CeguiTextureImpl(const CEGUI::String &name)
    : m_name(name)
{

}

CeguiTextureImpl::CeguiTextureImpl(const CEGUI::String &name, const CEGUI::String &filename, const CEGUI::String &resourceGroup)
    : CeguiTextureImpl(name)
{

}

CeguiTextureImpl::CeguiTextureImpl(const CEGUI::String &name, const CEGUI::Sizef &size)
    : CeguiTextureImpl(name)
{

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
    return false;
}

} } }