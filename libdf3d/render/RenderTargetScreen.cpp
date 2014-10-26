#include "df3d_pch.h"
#include "RenderTargetScreen.h"

#include <base/Controller.h>
#include "RenderManager.h"
#include "Viewport.h"
#include "OpenGLCommon.h"

namespace df3d { namespace render {

RenderTargetScreen::RenderTargetScreen(int width, int height)
    : m_width(width),
    m_height(height)
{

}

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
    return m_width;
}

int RenderTargetScreen::getHeight() const
{
    return m_height;
}

} }