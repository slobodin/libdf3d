#include "df3d.h"
#include "InputManager.h"

#include "InputEvents.h"
#include <base/EngineController.h>
#include <gui/GuiManager.h>
#include <gui/impl/RocketKeyCodesAdapter.h>

namespace df3d {

InputManager::InputSubscriber* InputManager::findSubscriber(InputListener *l)
{
    for (auto &it : m_inputListeners)
        if (it.valid && (it.listener->m_id == l->m_id))
            return &it;

    return nullptr;
}

void InputManager::registerInputListener(InputListener *listener)
{
    auto found = findSubscriber(listener);
    if (found)
    {
        glog << "Trying to add duplicate input listener" << logwarn;
        return;
    }

    InputSubscriber s;
    s.valid = true;
    s.listener = listener;
    m_inputListeners.push_back(s);
}

void InputManager::unregisterInputListener(InputListener *listener)
{
    auto found = findSubscriber(listener);
    if (found)
        found->valid = false;
    else
        glog << "Trying to remove nonexistent input listener." << logwarn;
}

void InputManager::pauseInput(bool pause)
{
    glog << "Not implemented" << logwarn;
}

void InputManager::onTouchEvent(const TouchEvent &touchEvent)
{
    // Force move event in order to pass coords to libRocket. wtf?
    if (touchEvent.id == 0)
        df3d::svc().guiManager().getContext()->ProcessMouseMove(touchEvent.x, touchEvent.y, 0);

    switch (touchEvent.state)
    {
    case df3d::TouchEvent::State::DOWN:
        if (touchEvent.id == 0)
            df3d::svc().guiManager().getContext()->ProcessMouseButtonDown(0, 0);
        break;
    case df3d::TouchEvent::State::UP:
        if (touchEvent.id == 0)
            df3d::svc().guiManager().getContext()->ProcessMouseButtonUp(0, 0);
        break;
    case df3d::TouchEvent::State::MOVING:
        break;
    case df3d::TouchEvent::State::CANCEL:
        break;
    default:
        break;
    }

    for (auto listener : m_inputListeners)
        if (listener.valid)
            listener.listener->onTouchEvent(touchEvent);
}

void InputManager::onMouseButtonEvent(const MouseButtonEvent &mouseButtonEvent)
{
    // Emulate a touch.
    if (mouseButtonEvent.button == MouseButtonEvent::Button::LEFT)
    {
        TouchEvent ev;
        ev.x = mouseButtonEvent.x;
        ev.y = mouseButtonEvent.y;
        if (mouseButtonEvent.state == MouseButtonEvent::State::PRESSED)
            ev.state = TouchEvent::State::DOWN;
        else if (mouseButtonEvent.state == MouseButtonEvent::State::RELEASED)
            ev.state = TouchEvent::State::UP;
        else
            return;

        onTouchEvent(ev);
    }

    for (auto listener : m_inputListeners)
        if (listener.valid)
            listener.listener->onMouseButtonEvent(mouseButtonEvent);
}

void InputManager::onMouseMotionEvent(const MouseMotionEvent &mouseMotionEvent)
{
    // Emulate a touch.
    if (mouseMotionEvent.leftPressed)
    {
        TouchEvent ev;
        ev.x = mouseMotionEvent.x;
        ev.y = mouseMotionEvent.y;
        ev.dx = mouseMotionEvent.dx;
        ev.dy = mouseMotionEvent.dy;
        ev.state = TouchEvent::State::MOVING;

        onTouchEvent(ev);
    }

    for (auto listener : m_inputListeners)
        if (listener.valid)
            listener.listener->onMouseMotionEvent(mouseMotionEvent);
}

void InputManager::onMouseWheelEvent(const MouseWheelEvent &mouseWheelEvent)
{
    df3d::svc().guiManager().getContext()->ProcessMouseWheel(static_cast<int>(mouseWheelEvent.delta), 0);
}

void InputManager::onKeyUp(const KeyboardEvent &keyUpEvent)
{
    auto rocketCode = gui_impl::convertToRocketKeyCode(keyUpEvent.keycode);
    auto rocketModifiers = gui_impl::convertToRocketModifier(keyUpEvent.modifiers);

    svc().guiManager().getContext()->ProcessKeyUp(rocketCode, rocketModifiers);

    for (auto listener : m_inputListeners)
        if (listener.valid)
            listener.listener->onKeyUp(keyUpEvent);
}

void InputManager::onKeyDown(const KeyboardEvent &keyDownEvent)
{
    auto rocketCode = gui_impl::convertToRocketKeyCode(keyDownEvent.keycode);
    auto rocketModifiers = gui_impl::convertToRocketModifier(keyDownEvent.modifiers);

    svc().guiManager().getContext()->ProcessKeyDown(rocketCode, rocketModifiers);

    for (auto listener : m_inputListeners)
        if (listener.valid)
            listener.listener->onKeyDown(keyDownEvent);
}

void InputManager::onTextInput(unsigned int codepoint)
{
    svc().guiManager().getContext()->ProcessTextInput(codepoint);
}

void InputManager::cleanInvalidListeners()
{
    for (auto it = m_inputListeners.cbegin(); it != m_inputListeners.cend(); )
    {
        if (!it->valid)
            it = m_inputListeners.erase(it);
        else
            it++;
    }
}

InputListener::InputListener()
{
    static uint32_t id;
    // Can't use just pointer of this object as an id.
    m_id = id++;

    svc().inputManager().registerInputListener(this);
}

InputListener::~InputListener()
{
    svc().inputManager().unregisterInputListener(this);
}

}
