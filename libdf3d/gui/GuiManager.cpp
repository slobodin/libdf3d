#include "df3d_pch.h"
#include "GuiManager.h"

#include "RocketInterface.h"
#include <base/EngineController.h>
#include <base/InputEvents.h>

#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <Rocket/Debugger.h>

namespace df3d { namespace gui {

GuiManager::GuiManager(int contextWidth, int contextHeight)
{
    base::glog << "Initializing libRocket" << base::logmess;

    using namespace Rocket;

    m_renderInterface.reset(new GuiRenderInterface());
    m_systemInterface.reset(new GuiSystemInterface());
    m_fileInterface.reset(new GuiFileInterface());

    SetRenderInterface(m_renderInterface.get());
    SetSystemInterface(m_systemInterface.get());
    SetFileInterface(m_fileInterface.get());

    // Initialize core.
    if (!Core::Initialise())
        throw std::runtime_error("Can not initialize Rocket GUI library");

    // Initialize controls library.
    Controls::Initialise();

    // Create GUI context.
    m_rocketContext = Core::CreateContext("main", Core::Vector2i(contextWidth, contextHeight));

    // Initialize debugger.
#ifdef ENABLE_ROCKET_DEBUGGER
    if (!Rocket::Debugger::Initialise(m_rocketContext))
        base::glog << "Failed to initialize Rocket GUI debugger" << base::logwarn;
#endif
}

GuiManager::~GuiManager()
{
    m_rocketContext->RemoveReference();
    Rocket::Core::Shutdown();
}

void GuiManager::update(float dt)
{
    m_rocketContext->Update();
}

void GuiManager::render()
{
    m_rocketContext->Render();
}

bool GuiManager::processMouseButtonEvent(const base::MouseButtonEvent &ev)
{
    int buttonIdx;
    switch (ev.button)
    {
    case base::MouseButtonEvent::Button::LEFT:
        buttonIdx = 0;
        break;
    case base::MouseButtonEvent::Button::RIGHT:
        buttonIdx = 1;
        break;
    case base::MouseButtonEvent::Button::MIDDLE:
    default:
        return false;
    }

    if (ev.state == base::MouseButtonEvent::State::PRESSED)
        m_rocketContext->ProcessMouseButtonDown(buttonIdx, 0);
    else if (ev.state == base::MouseButtonEvent::State::RELEASED)
        m_rocketContext->ProcessMouseButtonUp(buttonIdx, 0);

    return true;
}

bool GuiManager::processMouseMotionEvent(const base::MouseMotionEvent &ev)
{
    m_rocketContext->ProcessMouseMove(ev.x, ev.y, 0);
    return true;
}

bool GuiManager::processMouseWheelEvent(const base::MouseWheelEvent &ev)
{
    m_rocketContext->ProcessMouseWheel(ev.delta, 0);
    return true;
}

bool GuiManager::processKeyDownEvent(const base::KeyboardEvent::KeyCode &code)
{
    return false;
}

bool GuiManager::processKeyUpEvent(const base::KeyboardEvent::KeyCode &code)
{
    return false;
}

RocketDocument GuiManager::loadDocument(const char *name)
{
    auto doc = m_rocketContext->LoadDocument(name);
    if (!doc)
        return nullptr;

    doc->RemoveReference();
    return doc;
}

} }
