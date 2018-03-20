#pragma once

#include "InputEvents.h"

namespace df3d 
{

enum MfiControllerKind
{
    MFI_CONTROLLER_REMOTE,
    MFI_CONTROLLER_GAMEPAD
};

class MFiControllerListener
{
public:
    MFiControllerListener() = default;
    virtual ~MFiControllerListener() = default;

    virtual void MFiControllerConnected() { }
    virtual void MFiControllerDisconnected() { }

    virtual void Mfi_buttonA_Pressed(bool pressed) { }
    virtual void Mfi_buttonX_Pressed(bool pressed) { }
    virtual void Mfi_buttonY_Pressed(bool pressed) { }
    virtual void Mfi_buttonB_Pressed(bool pressed) { }
    virtual bool Mfi_buttonMenu_Pressed() { return false; }

    virtual void Mfi_DPadLeft_Pressed(bool pressed) { }
    virtual void Mfi_DPadRight_Pressed(bool pressed) { }
    virtual void Mfi_DPadUp_Pressed(bool pressed) { }
    virtual void Mfi_DPadDown_Pressed(bool pressed) { }

    virtual void Mfi_LeftShoulder_Changed(float value, bool pressed) { }
    virtual void Mfi_RightShoulder_Changed(float value, bool pressed) { }

    virtual void Mfi_LeftTrigger_Changed(float value, bool pressed) { }
    virtual void Mfi_RightTrigger_Changed(float value, bool pressed) { }

    virtual void Mfi_LeftThumbStick_Changed(float xValue, float yValue) { }
    virtual void Mfi_RightThumbStick_Changed(float xValue, float yValue) { }
};

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

    MFiControllerListener *m_listener = nullptr;
    std::unordered_map<uintptr_t, MfiControllerKind> m_controllers;

    void cleanStep();
    void processTouchDown(const Touch &touch);
    void processTouchUp(const Touch &touch);
    void processTouchMove(const Touch &touch);
    void processTouchCancel(const Touch &touch);

public:
    InputManager() = default;
    ~InputManager() = default;

    void setEnabled(bool enabled);
    bool getEnabled() const { return m_enabled; }

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

    MFiControllerListener* getMfiControllerListener() { return m_listener; }
    void setMfiControllerListener(MFiControllerListener *listener) { m_listener = listener; }

    void addController(uintptr_t controllerId, MfiControllerKind kind);
    void removeController(uintptr_t controllerId);
    bool anyMfiController() const;
    int controllersCount(MfiControllerKind kind) const;
};

}
