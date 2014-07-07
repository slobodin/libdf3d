#include "df3d_pch.h"
#include "RenderTargetScreen.h"

#include <base/Controller.h>
#include "RenderManager.h"
#include "Viewport.h"
#include "OpenGLCommon.h"

namespace df3d { namespace render {

void RenderTargetScreen::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTargetScreen::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int RenderTargetScreen::getWidth() const
{
    return g_renderManager->getViewport()->width();
}

int RenderTargetScreen::getHeight() const
{
    return g_renderManager->getViewport()->height();
}

} }