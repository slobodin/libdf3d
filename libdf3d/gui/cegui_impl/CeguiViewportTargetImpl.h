#pragma once

#include "CeguiRenderTargetImpl.h"

namespace df3d { namespace gui { namespace cegui_impl {

class CeguiViewportTargetImpl : public CeguiRenderTargetImpl<CEGUI::RenderTarget>
{
public:
    CeguiViewportTargetImpl(CeguiRendererImpl &owner, int width, int height);
    virtual ~CeguiViewportTargetImpl();

    bool isImageryCache() const;
};

} } }
