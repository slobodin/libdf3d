#include "df3d_pch.h"
#include "GuiManager.h"

#include <base/Controller.h>
#include <base/InputEvents.h>
#include <scripting/ScriptManager.h>

#include <CEGUI/CEGUI.h>
#include "cegui_impl/CeguiRendererImpl.h"
#include "cegui_impl/CeguiResourceProviderImpl.h"

namespace df3d { namespace gui {

GuiManager::GuiManager()
{
}

GuiManager::~GuiManager()
{
}

bool GuiManager::init(int contextWidth, int contextHeight)
{
    base::glog << "Initializing CEGUI" << base::logmess;

    try
    {
        cegui_impl::CeguiRendererImpl::bootstrapSystem(contextWidth, contextHeight);
    }
    catch (const CEGUI::Exception &e)
    {
        base::glog << "Failed to init CEGUI. Reason:" << e.what() << base::logcritical;
        return false;
    }

    return true;
}

void GuiManager::shutdown()
{
    try
    {
        cegui_impl::CeguiRendererImpl::destroySystem();
    }
    catch (const CEGUI::Exception &e)
    {
        base::glog << "Failed to destroy CEGUI. Reason:" << e.what() << base::logcritical;
        throw;
    }
}

void GuiManager::update(float dt)
{
    CEGUI::System::getSingleton().getDefaultGUIContext().injectTimePulse(dt);
}

void GuiManager::render()
{
    CEGUI::System::getSingleton().renderAllGUIContexts();
}

void GuiManager::processMouseButtonEvent(const SDL_MouseButtonEvent &ev)
{
    CEGUI::MouseButton mb;
    switch (ev.button)
    {
    case SDL_BUTTON_LEFT:
        mb = CEGUI::LeftButton;
        break;
    case SDL_BUTTON_RIGHT:
        mb = CEGUI::RightButton;
        break;
    case SDL_BUTTON_MIDDLE:
        mb = CEGUI::MiddleButton;
        break;
    case SDL_BUTTON_X1:
        mb = CEGUI::X1Button;
        break;
    case SDL_BUTTON_X2:
        mb = CEGUI::X2Button;
        break;
    default:
        return;
    }

    if (ev.type == SDL_MOUSEBUTTONDOWN)
        CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(mb);
    else if (ev.type == SDL_MOUSEBUTTONUP)
        CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(mb);
}

void GuiManager::processMouseMotionEvent(const base::MouseMotionEvent &ev)
{
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition((float)ev.x, (float)ev.y);
}

void GuiManager::processMouseWheelEvent(const SDL_MouseWheelEvent &ev)
{
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseWheelChange((float)ev.y);
}

void GuiManager::processKeyDownEvent(const SDL_KeyboardEvent &ev)
{
}

void GuiManager::processKeyUpEvent(const SDL_KeyboardEvent &ev)
{
}

void GuiManager::processTextInputEvent(const SDL_TextInputEvent &ev)
{
}

void GuiManager::setResourceGroupDirectory(const char *resourceGroup, const char *directory)
{
    auto rp = static_cast<cegui_impl::CeguiResourceProviderImpl*>(CEGUI::System::getSingleton().getResourceProvider());
    rp->setResourceGroupDirectory(resourceGroup, directory);
}

} }
