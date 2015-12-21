#include "df3d.h"
#include "InputManager.h"

#include "InputEvents.h"
#include <base/EngineController.h>
#include <gui/GuiManager.h>
#include <gui/impl/RocketKeyCodesAdapter.h>

namespace df3d {

void InputManager::cleanStep()
{
    m_prevMouseState = m_mouseState;
    m_prevKeyboardState = m_keyboardState;

    m_mouseState.wheelDelta = 0.0f;
    m_touches.clear();
}

const std::vector<Touch>& InputManager::getTouches() const
{
    return m_touches;
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

void InputManager::onMouseButtonPressed(MouseButton button)
{
    df3d::svc().guiManager().getContext()->ProcessMouseButtonDown(gui_impl::convertToRocketMouseButtonIdx(button), 0);

    m_mouseState.buttons.at((size_t)button) = MouseState::PRESSED;
}

void InputManager::onMouseButtonReleased(MouseButton button)
{
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

void InputManager::onTouch(const Touch &touch)
{
    // TODO: implement.
    assert(false);

    // Force move event in order to pass coords to libRocket. wtf?
    if (touch.id == 0)
        df3d::svc().guiManager().getContext()->ProcessMouseMove(touch.x, touch.y, 0);

    switch (touch.state)
    {
    case Touch::State::DOWN:
        if (touch.id == 0)
            df3d::svc().guiManager().getContext()->ProcessMouseButtonDown(0, 0);
        break;
    case Touch::State::UP:
        if (touch.id == 0)
            df3d::svc().guiManager().getContext()->ProcessMouseButtonUp(0, 0);
        break;
    default:
        break;
    }
}

}
