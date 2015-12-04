#pragma once

namespace df3d {

class DF3D_DLL MouseMotionEvent
{
public:
    bool leftPressed = false;
    bool rightPressed = false;
    int x = 0;
    int y = 0;
    int dx = 0;
    int dy = 0;
};

class DF3D_DLL MouseButtonEvent
{
public:
    enum class State
    {
        PRESSED,
        RELEASED
    };

    enum class Button
    {
        LEFT,
        RIGHT,
        MIDDLE
    };

    State state;
    Button button;
    int x = 0;
    int y = 0;
};

class DF3D_DLL MouseWheelEvent
{
public:
    float delta = 0;
};

class DF3D_DLL KeyboardEvent
{
public:
    enum KeyCode
    {
        KEY_F1,
        KEY_F2,
        KEY_F3,
        KEY_F4,
        KEY_F5,
        KEY_F6,
        KEY_F7,
        KEY_F8,
        KEY_F9,
        KEY_F10,
        KEY_F11,
        KEY_F12,

        KEY_0,
        KEY_1,
        KEY_2,
        KEY_3,
        KEY_4,
        KEY_5,
        KEY_6,
        KEY_7,
        KEY_8,
        KEY_9,

        KEY_A,
        KEY_B,
        KEY_C,
        KEY_D,
        KEY_E,
        KEY_F,
        KEY_G,
        KEY_H,
        KEY_I,
        KEY_J,
        KEY_K,
        KEY_L,
        KEY_M,
        KEY_N,
        KEY_O,
        KEY_P,
        KEY_Q,
        KEY_R,
        KEY_S,
        KEY_T,
        KEY_U,
        KEY_V,
        KEY_W,
        KEY_X,
        KEY_Y,
        KEY_Z,
        KEY_LEFT_BRACKET,
        KEY_RIGHT_BRACKET,
        KEY_GRAVE_ACCENT,
        KEY_SLASH,
        KEY_BACKSLASH,
        KEY_SPACE,
        KEY_EQUAL,
        KEY_MINUS,

        KEY_ESCAPE,
        KEY_ENTER,
        KEY_TAB,
        KEY_BACKSPACE,
        KEY_INSERT,
        KEY_DELETE,
        KEY_LEFT,
        KEY_RIGHT,
        KEY_UP,
        KEY_DOWN,
        KEY_PAGE_UP,
        KEY_PAGE_DOWN,
        KEY_HOME,
        KEY_END,
        KEY_CAPS_LOCK,
        KEY_SCROLL_LOCK,
        KEY_NUM_LOCK,
        KEY_PRINT_SCREEN,
        KEY_PAUSE,
        
        KEY_LEFT_SHIFT,
        KEY_LEFT_ALT,
        KEY_LEFT_CTRL,

        KEY_RIGHT_SHIFT,
        KEY_RIGHT_ALT,
        KEY_RIGHT_CTRL,

        UNDEFINED
    };

    enum KeyModifier
    {
        KM_NONE = 0,
        KM_CTRL = 1 << 0,
        KM_ALT = 1 << 1,
        KM_SHIFT = 1 << 2
    };

    KeyCode keycode;
    KeyModifier modifiers;
};

class DF3D_DLL TouchEvent
{
public:
    enum class State
    {
        UP,
        DOWN,
        MOVING,
        CANCEL
    };

    int id = 0;
    int x = 0;
    int y = 0;
    int dx = 0;
    int dy = 0;
    State state;
};

}
