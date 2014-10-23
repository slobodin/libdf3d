#include "df3d_pch.h"
#include "GuiManager.h"

#include <base/Controller.h>
#include <base/InputEvents.h>
#include <scripting/ScriptManager.h>

#include <CEGUI/CEGUI.h>
#include "cegui_impl/CeguiRendererImpl.h"

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
        cegui_impl::CeguiRendererImpl::bootstrapSystem();
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

}

void GuiManager::update(float dt)
{

}

void GuiManager::render()
{
    CEGUI::System::getSingleton().renderAllGUIContexts();
}

void GuiManager::processMouseButtonEvent(const SDL_MouseButtonEvent &ev)
{
}

void GuiManager::processMouseMotionEvent(const base::MouseMotionEvent &ev)
{
}

void GuiManager::processMouseWheelEvent(const SDL_MouseWheelEvent &ev)
{
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

} }
