#include "df3d_pch.h"
#include "CeguiViewportTargetImpl.h"

#include <base/Controller.h>
#include <render/RenderManager.h>
#include <render/RenderTargetScreen.h>

namespace df3d { namespace gui { namespace cegui_impl {

CeguiViewportTargetImpl::CeguiViewportTargetImpl(CeguiRendererImpl &owner)
    : CeguiRenderTargetImpl<CEGUI::RenderTarget>(owner)
{
    m_rt = g_renderManager->getScreenRenderTarget();

    CEGUI::Rectf initArea(0.0f, 0.0f, (float)m_rt->getWidth(), (float)m_rt->getHeight());
    setArea(initArea);
}

CeguiViewportTargetImpl::~CeguiViewportTargetImpl()
{
}

bool CeguiViewportTargetImpl::isImageryCache() const
{
    return false;
}

} } }