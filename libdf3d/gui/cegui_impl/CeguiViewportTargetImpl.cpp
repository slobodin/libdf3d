#include "df3d_pch.h"
#include "CeguiViewportTargetImpl.h"

#include <base/EngineController.h>
#include <render/RenderManager.h>
#include <render/RenderTargetScreen.h>
#include <render/Viewport.h>

namespace df3d { namespace gui { namespace cegui_impl {

CeguiViewportTargetImpl::CeguiViewportTargetImpl(CeguiRendererImpl &owner)
    : CeguiRenderTargetImpl<CEGUI::RenderTarget>(owner)
{
    m_rt = g_renderManager->getScreenRenderTarget();

    const auto &vp = m_rt->getViewport();

    CEGUI::Rectf initArea((float)vp.x(), (float)vp.y(), (float)vp.width(), (float)vp.height());
    setArea(initArea);
}

CeguiViewportTargetImpl::~CeguiViewportTargetImpl()
{
}

bool CeguiViewportTargetImpl::isImageryCache() const
{
    return false;
}

void CeguiViewportTargetImpl::setArea(const CEGUI::Rectf &area)
{
    render::Viewport newvp(area.left(), area.top(), area.right(), area.bottom());
    m_rt->setViewport(newvp);

    CeguiRenderTargetImpl<CEGUI::RenderTarget>::setArea(area);
}

} } }