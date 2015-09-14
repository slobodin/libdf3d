#include "df3d_pch.h"
#include "RenderTargetScreen.h"

#include <base/Service.h>
#include "RenderManager.h"
#include "OpenGLCommon.h"
#include "RendererBackend.h"

namespace df3d { namespace render {

RenderTargetScreen::RenderTargetScreen(const Viewport &vp)
{
    m_viewport = vp;
}

void RenderTargetScreen::bind()
{
    gsvc().renderMgr.getRenderer()->setViewport(m_viewport);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTargetScreen::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} }
