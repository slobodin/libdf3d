#include "RenderTargetScreen.h"

#include <base/EngineController.h>
#include "RenderManager.h"
#include "OpenGLCommon.h"
#include "RendererBackend.h"

namespace df3d {

RenderTargetScreen::RenderTargetScreen(const Viewport &vp)
{
    m_viewport = vp;
}

void RenderTargetScreen::bind()
{
    svc().renderManager().getRenderer()->setViewport(m_viewport);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTargetScreen::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}
