#pragma once

#include "RenderTarget.h"

namespace df3d { namespace render {

class RenderTargetScreen : public RenderTarget
{
public:
    RenderTargetScreen(const Viewport &vp);

    void bind();
    void unbind();
};

} }