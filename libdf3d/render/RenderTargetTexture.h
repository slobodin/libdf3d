#pragma once

#include "RenderTarget.h"

namespace df3d { namespace render {

class Texture;

class RenderTargetTexture : public RenderTarget
{
    unsigned m_fbo = 0;
    unsigned m_renderBufferId = 0;

    shared_ptr<Texture> m_texture;
    void createGLFramebuffer();

public:
    RenderTargetTexture(shared_ptr<Texture> texture);
    ~RenderTargetTexture();

    void bind();
    void unbind();

    int getWidth() const;
    int getHeight() const;

    shared_ptr<Texture> getTexture();
};

} }