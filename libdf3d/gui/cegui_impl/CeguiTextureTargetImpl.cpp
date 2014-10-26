#include "df3d_pch.h"
#include "CeguiTextureTargetImpl.h"

#include "CeguiTextureImpl.h"
#include <render/Texture.h>
#include <render/Image.h>
#include <render/RenderTargetTexture.h>

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
    declareRenderSize({ 128.0f, 128.0f });
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
    if ((m_area.getWidth() >= sz.d_width) && (m_area.getHeight() >= sz.d_height))
        return;

    CEGUI::Rectf init_area(CEGUI::Vector2f(0, 0), CEGUI::Sizef(sz.d_width, sz.d_height));

    setArea(init_area);
}

bool CeguiTextureTargetImpl::isRenderingInverted() const
{
    return true;
}

void CeguiTextureTargetImpl::setArea(const CEGUI::Rectf &area)
{
    m_texture = &static_cast<CeguiTextureImpl &>(m_owner.createTexture(generateTextureName(), area.getSize()));

    auto image = make_shared<render::Image>();

    image->setWidth(area.getWidth());
    image->setHeight(area.getHeight());
    image->setPixelFormat(render::Image::PF_RGBA);
    image->setInitialized();

    m_texture->getDf3dTexture()->setImage(image);

    m_rt = make_shared<render::RenderTargetTexture>(m_texture->getDf3dTexture());

    CeguiRenderTargetImpl<CEGUI::TextureTarget>::setArea(area);
}

} } }