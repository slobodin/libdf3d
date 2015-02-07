#include "df3d_pch.h"
#include "RenderTargetScreen.h"

#include <base/EngineController.h>
#include "RenderManager.h"
#include "OpenGLCommon.h"
#include "Renderer.h"

namespace df3d { namespace render {

RenderTargetScreen::RenderTargetScreen(const Viewport &vp)
{
    m_viewport = vp;
}

void RenderTargetScreen::bind()
{
    g_renderManager->getRenderer()->setViewport(m_viewport);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTargetScreen::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} }