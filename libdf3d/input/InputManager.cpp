#include "InputManager.h"

#include "InputEvents.h"
#include <libdf3d/base/EngineController.h>
#include <libdf3d/gui/GuiManager.h>
#include <libdf3d/gui/impl/RocketKeyCodesAdapter.h>

namespace df3d {

const size_t InputManager::MAX_TOUCHES = 16;

void InputManager::cleanStep()
{
    m_prevMouseState = m_mouseState;
    m_prevKeyboardState = m_keyboardState;

    m_mouseState.wheelDelta = 0.0f;
    m_mouseState.delta = glm::ivec2(0, 0);

    m_touchesCount = 0;
    for (const auto &kv : m_touches)
    {
        m_currentTouches[m_touchesCount++] = kv.second;
        if (m_touchesCount >= MAX_TOUCHES)
        {
            glog << "Touch limit exceeded" << logwarn;
            break;
        }
    }

    for (auto it = m_touches.begin(); it != m_touches.end(); )
    {
        if (it->second.state == Touch::State::UP || it->second.state == Touch::State::CANCEL)
            it = m_touches.erase(it);
        else
            it++;
    }
}

const Touch& InputManager::getTouch(size_t idx) const
{
    assert(idx >= 0 && idx < getTouchesCount());
    return m_currentTouches[idx];
}

size_t InputManager::getTouchesCount() const
{
    return m_touchesCount;
}

const glm::ivec2& InputManager::getMousePosition() const
{
    return m_mouseState.position;
}

const glm::ivec2& InputManager::getMouseDelta() const
{
    return m_mouseState.delta;
}

bool InputManager::getMouseButton(MouseButton button) const
{
    return m_mouseState.buttons.at((size_t)button) == MouseState::PRESSED;
}

bool InputManager::getMouseButtonPressed(MouseButton button) const
{
    return (m_prevMouseState.buttons.at((size_t)button) == MouseState::RELEASED) && getMouseButton(button);
}

bool InputManager::getMouseButtonReleased(MouseButton button) const
{
    return (m_prevMouseState.buttons.at((size_t)button) == MouseState::PRESSED) && !getMouseButton(button);
}

float InputManager::getMouseWheelDelta() const
{
    return m_mouseState.wheelDelta;
}

bool InputManager::getKey(KeyCode key) const
{
    return m_keyboardState.keyboard.at((size_t)key) == KeyboardState::PRESSED;
}

bool InputManager::getKeyPressed(KeyCode key) const
{
    return (m_prevKeyboardState.keyboard.at((size_t)key) == KeyboardState::RELEASED) && getKey(key);
}

bool InputManager::getKeyReleased(KeyCode key) const
{
    return (m_prevKeyboardState.keyboard.at((size_t)key) == KeyboardState::PRESSED) && !getKey(key);
}

KeyModifier InputManager::getKeyModifiers() const
{
    return m_keyboardState.modifiers;
}

void InputManager::onMouseButtonPressed(MouseButton button, int x, int y)
{
    setMousePosition(x, y);

    df3d::svc().guiManager().getContext()->ProcessMouseButtonDown(gui_impl::convertToRocketMouseButtonIdx(button), 0);

    m_mouseState.buttons.at((size_t)button) = MouseState::PRESSED;
}

void InputManager::onMouseButtonReleased(MouseButton button, int x, int y)
{
    setMousePosition(x, y);

    df3d::svc().guiManager().getContext()->ProcessMouseButtonUp(gui_impl::convertToRocketMouseButtonIdx(button), 0);

    m_mouseState.buttons.at((size_t)button) = MouseState::RELEASED;
}

void InputManager::setMousePosition(int x, int y)
{
    m_mouseState.position = glm::ivec2(x, y);
    m_mouseState.delta = m_mouseState.position - m_prevMouseState.position;
    // Force move event in order to pass coords to libRocket. wtf?
    df3d::svc().guiManager().getContext()->ProcessMouseMove(x, y, 0);
}

void InputManager::setMouseWheelDelta(float delta)
{
    df3d::svc().guiManager().getContext()->ProcessMouseWheel(static_cast<int>(delta), 0);
    m_mouseState.wheelDelta = delta;
}

void InputManager::onKeyUp(const KeyCode &keyCode, KeyModifier modifiers)
{
    auto rocketCode = gui_impl::convertToRocketKeyCode(keyCode);
    auto rocketModifiers = gui_impl::convertToRocketModifier(modifiers);

    svc().guiManager().getContext()->ProcessKeyUp(rocketCode, rocketModifiers);

    m_keyboardState.keyboard.at((size_t)keyCode) = KeyboardState::RELEASED;
}

void InputManager::onKeyDown(const KeyCode &keyCode, KeyModifier modifiers)
{
    auto rocketCode = gui_impl::convertToRocketKeyCode(keyCode);
    auto rocketModifiers = gui_impl::convertToRocketModifier(modifiers);

    svc().guiManager().getContext()->ProcessKeyDown(rocketCode, rocketModifiers);

    m_keyboardState.keyboard.at((size_t)keyCode) = KeyboardState::PRESSED;
}

void InputManager::onTextInput(unsigned int codepoint)
{
    svc().guiManager().getContext()->ProcessTextInput(codepoint);
}

void InputManager::onTouch(int id, int x, int y, Touch::State state)
{
    switch (state)
    {
    case Touch::State::DOWN:
    {
        auto found = m_touches.find(id);
        if (found == m_touches.end())
        {
            Touch newTouch;
            newTouch.state = state;
            newTouch.x = x;
            newTouch.y = y;
            newTouch.id = id;

            m_touches.insert({ id, newTouch});
        }
        else
        {
            glog << "InputManager::onTouch touchdown failed, already have this touch" << logwarn;
        }
    }
        break;
    case Touch::State::MOVING:
    {
        auto found = m_touches.find(id);
        if (found != m_touches.end())
        {
            found->second.state = state;
            found->second.dx = x - found->second.x;
            found->second.dy = y - found->second.y;
            found->second.x = x;
            found->second.y = y;
        }
        else
        {
            glog << "InputManager::onTouch touchmove failed, no such touch" << logwarn;
        }
    }
        break;
    case Touch::State::UP:
    case Touch::State::CANCEL:
    {
        auto found = m_touches.find(id);
        if (found != m_touches.end())
        {
            found->second.state = state;
            found->second.x = x;
            found->second.y = y;
        }
        else
        {
            glog << "InputManager::onTouch touchup failed, no such touch" << logwarn;
        }
    }
        break;
    default:
        break;
    }
}

}
