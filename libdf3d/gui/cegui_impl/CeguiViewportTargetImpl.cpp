#include "df3d_pch.h"
#include "CeguiViewportTargetImpl.h"

namespace df3d { namespace gui { namespace cegui_impl {

CeguiViewportTargetImpl::CeguiViewportTargetImpl(CeguiRendererImpl &owner, int width, int height)
    : CeguiRenderTargetImpl<CEGUI::RenderTarget>(owner)
{
    CEGUI::Rectf initArea(0.0f, 0.0f, (float)width, (float)height);
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