#pragma once

#include "CeguiRenderTargetImpl.h"

namespace df3d { namespace gui { namespace cegui_impl {

class CeguiViewportTargetImpl : public CeguiRenderTargetImpl<CEGUI::RenderTarget>
{
public:
    CeguiViewportTargetImpl(CeguiRendererImpl &owner);
    virtual ~CeguiViewportTargetImpl();

    bool isImageryCache() const;
    void setArea(const CEGUI::Rectf &area);
};

} } }
