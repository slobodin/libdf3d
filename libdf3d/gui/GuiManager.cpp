#include "df3d_pch.h"
#include "GuiManager.h"

#include <base/EngineController.h>
#include <base/InputEvents.h>

#include <CEGUI/CEGUI.h>
#include "cegui_impl/CeguiRendererImpl.h"
#include "cegui_impl/CeguiResourceProviderImpl.h"

namespace df3d { namespace gui {

GuiManager::GuiManager(int contextWidth, int contextHeight)
{
    base::glog << "Initializing CEGUI" << base::logmess;

    cegui_impl::CeguiRendererImpl::bootstrapSystem(contextWidth, contextHeight);
}

GuiManager::~GuiManager()
{
    try
    {
        cegui_impl::CeguiRendererImpl::destroySystem();
    }
    catch (const CEGUI::Exception &e)
    {
        base::glog << "Failed to destroy CEGUI. Reason:" << e.what() << base::logcritical;
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

void GuiManager::setResourceGroupDirectory(const char *resourceGroup, const char *directory)
{
    auto rp = static_cast<cegui_impl::CeguiResourceProviderImpl*>(CEGUI::System::getSingleton().getResourceProvider());
    rp->setResourceGroupDirectory(resourceGroup, directory);
}

bool GuiManager::processMouseButtonEvent(const base::MouseButtonEvent &ev)
{
    CEGUI::MouseButton mb;
    switch (ev.button)
    {
    case base::MouseButtonEvent::Button::LEFT:
        mb = CEGUI::LeftButton;
        break;
    case base::MouseButtonEvent::Button::RIGHT:
        mb = CEGUI::RightButton;
        break;
    case base::MouseButtonEvent::Button::MIDDLE:
        mb = CEGUI::MiddleButton;
    default:
        return false;
    }

    if (ev.state == base::MouseButtonEvent::State::PRESSED)
        return CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(mb);
    else if (ev.state == base::MouseButtonEvent::State::RELEASED)
        return CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(mb);

    return false;
}

bool GuiManager::processMouseMotionEvent(const base::MouseMotionEvent &ev)
{
    return CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition((float)ev.x, (float)ev.y);
}

bool GuiManager::processMouseWheelEvent(const base::MouseWheelEvent &ev)
{
    return CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseWheelChange(ev.delta);
}

bool GuiManager::processKeyDownEvent(const base::KeyboardEvent::KeyCode &code)
{
    return false;
}

bool GuiManager::processKeyUpEvent(const base::KeyboardEvent::KeyCode &code)
{
    return false;
}

} }
