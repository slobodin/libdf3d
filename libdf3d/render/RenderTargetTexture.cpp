#include "df3d_pch.h"
#include "RenderTargetTexture.h"

#include "OpenGLCommon.h"
#include "Texture.h"
#include "Image.h"

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
        base::glog << "Framebuffer initialization error:" << base::logwarn;
    }

    unbind();
}

RenderTargetTexture::RenderTargetTexture(shared_ptr<Texture> texture)
    : m_texture(texture)
{
    //assert(m_texture->valid());
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

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    m_texture->bind(0);
}

void RenderTargetTexture::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    m_texture->unbind();
}

int RenderTargetTexture::getWidth() const
{
    return m_texture->getOriginalWidth();
}

int RenderTargetTexture::getHeight() const
{
    return m_texture->getOriginalHeight();
}

shared_ptr<Texture> RenderTargetTexture::getTexture()
{
    return m_texture;
}

} }