#include "df3d_pch.h"
#include "CeguiTextureTargetImpl.h"

#include "CeguiTextureImpl.h"

namespace df3d { namespace gui { namespace cegui_impl {

CEGUI::String generateTextureName()
{
    static CEGUI::uint textureId = 0;
    CEGUI::String tmp("_libdf3d_tt_tex_");
    tmp.append(CEGUI::PropertyHelper<CEGUI::uint>::toString(textureId++));

    return tmp;
}

CeguiTextureTargetImpl::CeguiTextureTargetImpl(CeguiRendererImpl &owner)
    : CeguiRenderTargetImpl<CEGUI::TextureTarget>(owner)
{
    m_texture = &static_cast<CeguiTextureImpl &>(m_owner.createTexture(generateTextureName(), m_area.getSize()));
}

CeguiTextureTargetImpl::~CeguiTextureTargetImpl()
{
    m_owner.destroyTexture(*m_texture);
}

bool CeguiTextureTargetImpl::isImageryCache() const
{
    return true;
}

void CeguiTextureTargetImpl::clear()
{

}

CEGUI::Texture& CeguiTextureTargetImpl::getTexture() const
{
    return *m_texture;
}

void CeguiTextureTargetImpl::declareRenderSize(const CEGUI::Sizef &sz)
{

}

bool CeguiTextureTargetImpl::isRenderingInverted() const
{
    return false;
}

} } }