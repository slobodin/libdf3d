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
    m_rt->bind();
    g_renderManager->getRenderer()->clearColorBuffer(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
    m_rt->unbind();
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
    if (m_texture)
        m_owner.destroyTexture(*m_texture);

    auto vp = render::Viewport((int)area.left(), (int)area.top(), (int)area.right(), (int)area.bottom());
    m_rt = make_shared<render::RenderTargetTexture>(vp);

    auto df3dTexture = static_pointer_cast<render::RenderTargetTexture>(m_rt)->getTexture();

    m_texture = &m_owner.createTexture(generateTextureName(), df3dTexture);

    CeguiRenderTargetImpl<CEGUI::TextureTarget>::setArea(area);
}

} } }