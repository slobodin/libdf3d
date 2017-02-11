#include "InputManager.h"

#include "InputEvents.h"
#include <df3d/engine/EngineController.h>
#include <df3d/engine/gui/GuiManager.h>
#include <tb_widgets.h>

namespace df3d {

static uint32_t g_indexBitsUsed = 0;
static int MAX_TOUCHES = 15;

static tb::SPECIAL_KEY GetTBSpecialKey(KeyCode keyCode)
{
    switch (keyCode)
    {
    case KeyCode::KEY_ESCAPE:
        return tb::TB_KEY_ESC;
    case KeyCode::KEY_UP:
        return tb::TB_KEY_UP;
    case KeyCode::KEY_DOWN:
        return tb::TB_KEY_DOWN;
    case KeyCode::KEY_LEFT:
        return tb::TB_KEY_LEFT;
    case KeyCode::KEY_RIGHT:
        return tb::TB_KEY_RIGHT;
    case KeyCode::KEY_PAGE_UP:
        return tb::TB_KEY_PAGE_UP;
    case KeyCode::KEY_PAGE_DOWN:
        return tb::TB_KEY_PAGE_DOWN;
    case KeyCode::KEY_HOME:
        return tb::TB_KEY_HOME;
    case KeyCode::KEY_END:
        return tb::TB_KEY_END;
    case KeyCode::KEY_TAB:
        return tb::TB_KEY_TAB;
    case KeyCode::KEY_BACKSPACE:
        return tb::TB_KEY_BACKSPACE;
    case KeyCode::KEY_INSERT:
        return tb::TB_KEY_INSERT;
    case KeyCode::KEY_DELETE:
        return tb::TB_KEY_DELETE;
    case KeyCode::KEY_ENTER:
        return tb::TB_KEY_ENTER;
    default:
        break;
    }

    return tb::TB_KEY_UNDEFINED;
}

static bool IsPrimaryTouch(Touch touch)
{
    return touch.id == 0;
}

static int GetUnUsedIndex()
{
    int temp = g_indexBitsUsed;
    
    for (int i = 0; i < MAX_TOUCHES; i++)
    {
        if (!(temp & 0x00000001)) 
        {
            g_indexBitsUsed |= (1 << i);
            return i;
        }
        
        temp >>= 1;
    }

    // all bits are used
    return -1;
}

static void RemoveUsedIndexBit(int index)
{
    if (index < 0 || index >= MAX_TOUCHES)
        return;
    
    uint32_t temp = 1 << index;
    temp = ~temp;
    g_indexBitsUsed &= temp;
}

void InputManager::cleanStep()
{
    m_prevMouseState = m_mouseState;
    m_prevKeyboardState = m_keyboardState;

    m_mouseState.wheelDelta = 0.0f;
    m_mouseState.delta = glm::ivec2(0, 0);
}

void InputManager::processTouchDown(const Touch &touch)
{
    if (IsPrimaryTouch(touch))
    {
        m_mouseState.buttons[(size_t)MouseButton::LEFT] = MouseState::PRESSED;
        setMousePosition(touch.x, touch.y);
    }

    int clickCount = IsPrimaryTouch(touch) ? 1 : 0;

    auto root = df3d::svc().guiManager().getRoot();
    if (root && root->GetIsInteractable())
        root->InvokeTouchDown(touch.x, touch.y, touch.id, clickCount, tb::TB_MODIFIER_NONE);
}

void InputManager::processTouchUp(const Touch &touch)
{
    if (IsPrimaryTouch(touch))
    {
        m_mouseState.buttons[(size_t)MouseButton::LEFT] = MouseState::RELEASED;
        setMousePosition(touch.x, touch.y);
    }

    auto root = df3d::svc().guiManager().getRoot();
    if (root && root->GetIsInteractable())
        root->InvokeTouchUp(touch.x, touch.y, touch.id, tb::TB_MODIFIER_NONE);
}

void InputManager::processTouchMove(const Touch &touch)
{
    if (IsPrimaryTouch(touch))
        setMousePosition(touch.x, touch.y);

    auto root = df3d::svc().guiManager().getRoot();
    if (root && root->GetIsInteractable())
        root->InvokeTouchMove(touch.x, touch.y, touch.id, tb::TB_MODIFIER_NONE);
//
//    found->second.dx = x - found->second.x;
//    found->second.dy = y - found->second.y;
}

void InputManager::processTouchCancel(const Touch &touch)
{
    if (IsPrimaryTouch(touch))
    {
        m_mouseState.buttons[(size_t)MouseButton::LEFT] = MouseState::RELEASED;
        setMousePosition(touch.x, touch.y);
    }

    auto root = df3d::svc().guiManager().getRoot();
    if (root && root->GetIsInteractable())
        root->InvokeTouchCancel(touch.id);
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

void InputManager::setMouseButtonPressed(MouseButton button, int x, int y)
{
    if (button == MouseButton::LEFT)
    {
        Touch touch;
        touch.x = x;
        touch.y = y;
        touch.state = Touch::State::DOWN;
        touch.id = 0;
        processTouchDown(touch);
    }

    m_mouseState.buttons[(size_t)button] = MouseState::PRESSED;
}

void InputManager::setMouseButtonReleased(MouseButton button, int x, int y)
{
    if (button == MouseButton::LEFT)
    {
        Touch touch;
        touch.x = x;
        touch.y = y;
        touch.state = Touch::State::UP;
        touch.id = 0;
        processTouchUp(touch);
    }

    m_mouseState.buttons[(size_t)button] = MouseState::RELEASED;
}

void InputManager::setMousePosition(int x, int y)
{
    m_mouseState.position = glm::ivec2(x, y);
    m_mouseState.delta = m_mouseState.position - m_prevMouseState.position;
}

void InputManager::setMouseWheelDelta(float delta)
{
    if (!m_enabled)
        return;
    if (auto root = svc().guiManager().getRoot())
    {
        if (root->GetIsInteractable())
            root->InvokeWheel(m_mouseState.position.x, m_mouseState.position.y, 0, static_cast<int>(delta), tb::TB_MODIFIER_NONE);
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
    if (auto root = svc().guiManager().getRoot())
    {
        if (root->GetIsInteractable())
        {
            root->InvokeKey(codepoint, tb::TB_KEY_UNDEFINED, tb::TB_MODIFIER_NONE, true);
            root->InvokeKey(codepoint, tb::TB_KEY_UNDEFINED, tb::TB_MODIFIER_NONE, false);
        }
    }
}

void InputManager::onTouch(uintptr_t id, int x, int y, Touch::State state)
{
    if (!m_enabled)
        return;

    switch (state)
    {
        case Touch::State::DOWN:
        {
            auto found = m_touches.find(id);
            if (found == m_touches.end())
            {
                auto newIdx = GetUnUsedIndex();
                if (newIdx != -1)
                {
                    Touch newTouch;
                    newTouch.dx = 0;
                    newTouch.dy = 0;
                    newTouch.x = x;
                    newTouch.y = y;
                    newTouch.state = state;
                    newTouch.id = newIdx;

                    m_touches[id] = newTouch;

                    processTouchDown(newTouch);
                }
            }
            else
                DFLOG_WARN("Duplicate touch!");
        }
            break;
        case Touch::State::UP:
        case Touch::State::CANCEL:
        {
            auto found = m_touches.find(id);
            if (found != m_touches.end())
            {
                found->second.x = x;
                found->second.y = y;
                found->second.state = state;

                if (state == Touch::State::UP)
                    processTouchUp(found->second);
                else
                    processTouchCancel(found->second);

                RemoveUsedIndexBit(found->second.id);
                m_touches.erase(found);
            }
            else
                DFLOG_WARN("Failed to erase a touch!");
        }
            break;
        case Touch::State::MOVING:
        {
            auto found = m_touches.find(id);
            if (found != m_touches.end())
            {
                found->second.x = x;
                found->second.y = y;
                found->second.state = state;
                processTouchMove(found->second);
            }
            else
                DFLOG_WARN("Failed to find a touch!");
        }
            break;
        default:
            break;
    }
}

}
