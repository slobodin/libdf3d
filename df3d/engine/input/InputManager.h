#pragma once

#include "InputEvents.h"

namespace df3d 
{

class InputManager : NonCopyable
{
    friend class EngineController;

    struct MouseState
    {
        glm::ivec2 position;
        glm::ivec2 delta;
        float wheelDelta = 0.0f;

        enum ButtonState
        {
            PRESSED,
            RELEASED,
        };

        ButtonState buttons[static_cast<size_t>(MouseButton::UNDEFINED)];

        MouseState()
        {
            for (auto &button : buttons)
                button = ButtonState::RELEASED;
        }
    };

    MouseState m_prevMouseState;
    MouseState m_mouseState;

    struct KeyboardState
    {
        enum KeyState
        {
            PRESSED,
            RELEASED
        };

        KeyState keyboard[static_cast<size_t>(KeyCode::UNDEFINED)];
        KeyModifier modifiers = KM_NONE;

        KeyboardState()
        {
            for (auto &k : keyboard)
                k = KeyState::RELEASED;
        }
    };

    KeyboardState m_prevKeyboardState;
    KeyboardState m_keyboardState;

    bool m_enabled = true;
    std::unordered_map<uintptr_t, Touch> m_touches;

    void cleanStep();
    void processTouchDown(const Touch &touch);
    void processTouchUp(const Touch &touch);
    void processTouchMove(const Touch &touch);
    void processTouchCancel(const Touch &touch);

public:
    InputManager() = default;
    ~InputManager() = default;

    void setEnabled(bool enabled);

    const glm::ivec2& getMousePosition() const;
    const glm::ivec2& getMouseDelta() const;
    bool getMouseButton(MouseButton button) const;
    bool getMouseButtonPressed(MouseButton button) const;
    bool getMouseButtonReleased(MouseButton button) const;
    float getMouseWheelDelta() const;

    bool getKey(KeyCode key) const;
    bool getKeyPressed(KeyCode key) const;
    bool getKeyReleased(KeyCode key) const;
    KeyModifier getKeyModifiers() const;

    // This should be called by the platform code only.
    // TODO: improve encapsulation!
    void setMouseButtonPressed(MouseButton button, int x, int y);
    void setMouseButtonReleased(MouseButton button, int x, int y);
    void setMousePosition(int x, int y);
    void setMouseWheelDelta(float delta);
    void onKeyUp(const KeyCode &keyCode, KeyModifier modifiers);
    void onKeyDown(const KeyCode &keyCode, KeyModifier modifiers);
    void onTextInput(unsigned int codepoint);
    // TODO: make InputInterface with only get functions. Game code will use it.
    // Instantiate input manager per platform with these methods (setmousepos, onTouch, etc).
    void onTouch(uintptr_t id, int x, int y, Touch::State state);
};

}
