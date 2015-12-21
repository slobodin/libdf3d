#pragma once

#include "InputEvents.h"

namespace df3d 
{

class DF3D_DLL InputManager : utils::NonCopyable
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

        std::vector<ButtonState> buttons = std::vector<ButtonState>((size_t)MouseButton::UNDEFINED, ButtonState::RELEASED);
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

        std::vector<KeyState> keyboard = std::vector<KeyState>((size_t)KeyCode::UNDEFINED, KeyState::RELEASED);
        KeyModifier modifiers = KM_NONE;
    };

    KeyboardState m_prevKeyboardState;
    KeyboardState m_keyboardState;

    std::vector<Touch> m_touches;

    void cleanStep();

public:
    InputManager() = default;
    ~InputManager() = default;

    const std::vector<Touch>& getTouches() const;

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
    //void onTouchEvent(const Touch &touchEvent);
    void onMouseButtonPressed(MouseButton button);
    void onMouseButtonReleased(MouseButton button);
    void setMousePosition(int x, int y);
    void setMouseWheelDelta(float delta);
    void onKeyUp(const KeyCode &keyCode, KeyModifier modifiers);
    void onKeyDown(const KeyCode &keyCode, KeyModifier modifiers);
    void onTextInput(unsigned int codepoint);
    void onTouch(const Touch &touch);
};

}
