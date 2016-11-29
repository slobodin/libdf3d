#include "InputManager.h"

#include "InputEvents.h"
#include <df3d/engine/EngineController.h>
#include <df3d/engine/gui/GuiManager.h>
#include <tb_widgets.h>

namespace df3d {

static tb::SPECIAL_KEY GetTBSpecialKey(KeyCode keyCode)
{
    auto tbKey = tb::TB_KEY_UNDEFINED;
    switch (keyCode)
    {
    case KeyCode::KEY_ESCAPE:
        tbKey = tb::TB_KEY_ESC;
        break;
    default:
        break;
    }

    return tbKey;
}

void InputManager::cleanStep()
{
    m_prevMouseState = m_mouseState;
    m_prevKeyboardState = m_keyboardState;

    m_mouseState.wheelDelta = 0.0f;
    m_mouseState.delta = glm::ivec2(0, 0);
}

void InputManager::setEnabled(bool enabled)
{
    m_enabled = enabled;
    if (!m_enabled)
    {
        cleanStep();
        m_prevMouseState = m_mouseState = {};
        m_prevKeyboardState = m_keyboardState = {};
    }
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
    return m_mouseState.buttons[(size_t)button] == MouseState::PRESSED;
}

bool InputManager::getMouseButtonPressed(MouseButton button) const
{
    return (m_prevMouseState.buttons[(size_t)button] == MouseState::RELEASED) && getMouseButton(button);
}

bool InputManager::getMouseButtonReleased(MouseButton button) const
{
    return (m_prevMouseState.buttons[(size_t)button] == MouseState::PRESSED) && !getMouseButton(button);
}

float InputManager::getMouseWheelDelta() const
{
    return m_mouseState.wheelDelta;
}

bool InputManager::getKey(KeyCode key) const
{
    return m_keyboardState.keyboard[(size_t)key] == KeyboardState::PRESSED;
}

bool InputManager::getKeyPressed(KeyCode key) const
{
    return (m_prevKeyboardState.keyboard[(size_t)key] == KeyboardState::RELEASED) && getKey(key);
}

bool InputManager::getKeyReleased(KeyCode key) const
{
    return (m_prevKeyboardState.keyboard[(size_t)key] == KeyboardState::PRESSED) && !getKey(key);
}

KeyModifier InputManager::getKeyModifiers() const
{
    return m_keyboardState.modifiers;
}

void InputManager::onMouseButtonPressed(MouseButton button, int x, int y)
{
    if (!m_enabled)
        return;
    setMousePosition(x, y);

    if (auto root = svc().guiManager().getRoot())
    {
        if (root->GetIsInteractable())
            root->InvokeTouchDown(x, y, 0, 1, tb::TB_MODIFIER_NONE);
    }

    m_mouseState.buttons[(size_t)button] = MouseState::PRESSED;
}

void InputManager::onMouseButtonReleased(MouseButton button, int x, int y)
{
    if (!m_enabled)
        return;
    setMousePosition(x, y);

    if (auto root = svc().guiManager().getRoot())
    {
        if (root->GetIsInteractable())
            root->InvokeTouchUp(x, y, 0, tb::TB_MODIFIER_NONE);
    }

    m_mouseState.buttons[(size_t)button] = MouseState::RELEASED;
}

void InputManager::setMousePosition(int x, int y)
{
    m_mouseState.position = glm::ivec2(x, y);
    m_mouseState.delta = m_mouseState.position - m_prevMouseState.position;

    if (auto root = svc().guiManager().getRoot())
    {
        if (root->GetIsInteractable())
        {
            if (tb::TBWidget::captured_widget)
                root->InvokeTouchMove(x, y, 0, tb::TB_MODIFIER_NONE);
        }
    }
}

void InputManager::setMouseWheelDelta(float delta)
{
    if (!m_enabled)
        return;
    if (auto root = svc().guiManager().getRoot())
    {
        if (root->GetIsInteractable())
            root->InvokeWheel(m_mouseState.position.x, m_mouseState.position.y, 0, delta, tb::TB_MODIFIER_NONE);
    }
    m_mouseState.wheelDelta = delta;
}

void InputManager::onKeyUp(const KeyCode &keyCode, KeyModifier modifiers)
{
    if (!m_enabled)
        return;
    if (auto root = svc().guiManager().getRoot())
    {
        if (root->GetIsInteractable())
            root->InvokeKey(0, GetTBSpecialKey(keyCode), tb::TB_MODIFIER_NONE, false);
    }

    m_keyboardState.keyboard[(size_t)keyCode] = KeyboardState::RELEASED;
}

void InputManager::onKeyDown(const KeyCode &keyCode, KeyModifier modifiers)
{
    if (!m_enabled)
        return;
    if (auto root = svc().guiManager().getRoot())
    {
        if (root->GetIsInteractable())
            root->InvokeKey(0, GetTBSpecialKey(keyCode), tb::TB_MODIFIER_NONE, true);
    }

    m_keyboardState.keyboard[(size_t)keyCode] = KeyboardState::PRESSED;
}

void InputManager::onTextInput(unsigned int codepoint)
{
    if (!m_enabled)
        return;
}

void InputManager::onTouch(TouchID id, int x, int y, Touch::State state)
{
    if (!m_enabled)
        return;
    if (m_listener)
    {
        Touch newTouch;
        newTouch.id = id;
        newTouch.x = x;
        newTouch.y = y;
        newTouch.state = state;

        m_listener->onTouch(newTouch);
    }

#ifdef _DEBUG
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
            DFLOG_WARN("InputManager::onTouch touchdown failed, already have this touch");
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
            DFLOG_WARN("InputManager::onTouch touchmove failed, no such touch");
        }
    }
        break;
    case Touch::State::UP:
    case Touch::State::CANCEL:
    {
        auto found = m_touches.find(id);
        if (found != m_touches.end())
        {
            m_touches.erase(found);
        }
        else
        {
            DFLOG_WARN("InputManager::onTouch touchup failed, no such touch");
        }
    }
        break;
    default:
        break;
    }
#endif
}

}
