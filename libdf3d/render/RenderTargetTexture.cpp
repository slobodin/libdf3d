#include "RenderTargetTexture.h"

#include <base/Service.h>
#include "OpenGLCommon.h"
#include "Texture2D.h"
#include "RendererBackend.h"

namespace df3d { namespace render {

void RenderTargetTexture::createGLFramebuffer()
{
    if (m_fbo)
        return;

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    m_texture->bind(0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->getGLId(), 0);

    glGenRenderbuffers(1, &m_renderBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_texture->getActualWidth(), m_texture->getActualHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderBufferId);

//#if defined(WIN32)
//    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
//    glDrawBuffers(1, drawBuffers);
//#endif

    checkGLError();

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        // TODO:
        // Error message.
        glog << "Framebuffer initialization error:" << logwarn;
    }

    unbind();
}

RenderTargetTexture::RenderTargetTexture(const Viewport &vp)
{
    setViewport(vp);
}

RenderTargetTexture::~RenderTargetTexture()
{
    unbind();

    if (m_fbo)
        glDeleteFramebuffers(1, &m_fbo);
    if (m_renderBufferId)
        glDeleteRenderbuffers(1, &m_renderBufferId);
}

void RenderTargetTexture::bind()
{
    createGLFramebuffer();

    if (!m_fbo)
        return;

    svc().renderMgr.getRenderer()->setViewport(m_viewport);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    m_texture->bind(0);
}

void RenderTargetTexture::unbind()
{
    if (m_fbo)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (m_renderBufferId)
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    m_texture->unbind();
}

void RenderTargetTexture::setViewport(const Viewport &vp)
{
    if (m_texture)
        svc().resourceMgr.unloadResource(m_texture);

    m_viewport = vp;

    TextureCreationParams params;
    params.setMipmapped(false);
    params.setFiltering(TextureFiltering::NEAREST);

    auto pb = make_unique<PixelBuffer>(m_viewport.width(), m_viewport.height(), PixelFormat::RGBA);

    m_texture = svc().resourceMgr.getFactory().createTexture(std::move(pb), params);
}

shared_ptr<Texture2D> RenderTargetTexture::getTexture()
{
    return m_texture;
}

} }
