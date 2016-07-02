#pragma once

#include "InputEvents.h"

namespace df3d 
{

class TouchListener
{
public:
    virtual ~TouchListener() = default;

    virtual void onTouch(const Touch &touch) = 0;
};

class DF3D_DLL InputManager : NonCopyable
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

    std::unordered_map<TouchID, Touch> m_touches;
    TouchListener *m_listener = nullptr;

    void cleanStep();

public:
    InputManager() = default;
    ~InputManager() = default;

    void setTouchesListener(TouchListener *listener) { m_listener = listener; }

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
    void onMouseButtonPressed(MouseButton button, int x, int y);
    void onMouseButtonReleased(MouseButton button, int x, int y);
    void setMousePosition(int x, int y);
    void setMouseWheelDelta(float delta);
    void onKeyUp(const KeyCode &keyCode, KeyModifier modifiers);
    void onKeyDown(const KeyCode &keyCode, KeyModifier modifiers);
    void onTextInput(unsigned int codepoint);
    void onTouch(TouchID id, int x, int y, Touch::State state);
};

}
