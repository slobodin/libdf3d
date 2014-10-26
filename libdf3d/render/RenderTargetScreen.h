#pragma once

#include "RenderTarget.h"

namespace df3d { namespace render {

class RenderTargetScreen : public RenderTarget
{
    int m_width = 0;
    int m_height = 0;

public:
    RenderTargetScreen(int width, int height);

    void bind();
    void unbind();

    int getWidth() const;
    int getHeight() const;
};

} }