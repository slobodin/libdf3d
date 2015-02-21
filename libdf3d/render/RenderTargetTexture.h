#pragma once

#include "RenderTarget.h"

namespace df3d { namespace render {

class Texture2D;

class RenderTargetTexture : public RenderTarget
{
    unsigned m_fbo = 0;
    unsigned m_renderBufferId = 0;

    shared_ptr<Texture2D> m_texture;
    void createGLFramebuffer();

public:
    // Viewport is a render target size.
    RenderTargetTexture(const Viewport &vp);
    ~RenderTargetTexture();

    void bind() override;
    void unbind() override;

    void setViewport(const Viewport &vp) override;

    shared_ptr<Texture2D> getTexture();
};

} }
