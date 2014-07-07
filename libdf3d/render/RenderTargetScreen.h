#pragma once

#include "RenderTarget.h"

namespace df3d { namespace render {

class RenderTargetScreen : public RenderTarget
{
public:
    void bind();
    void unbind();

    int getWidth() const;
    int getHeight() const;
};

} }