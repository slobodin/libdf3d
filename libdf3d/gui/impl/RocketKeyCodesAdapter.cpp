#include "RocketKeyCodesAdapter.h"

namespace df3d { namespace gui_impl {

Rocket::Core::Input::KeyIdentifier convertToRocketKeyCode(KeyCode keycode)
{
    switch (keycode)
    {
    case KeyCode::KEY_F1:
        return Rocket::Core::Input::KeyIdentifier::KI_F1;
    case KeyCode::KEY_F2:
        return Rocket::Core::Input::KeyIdentifier::KI_F2;
    case KeyCode::KEY_F3:
        return Rocket::Core::Input::KeyIdentifier::KI_F3;
    case KeyCode::KEY_F4:
        return Rocket::Core::Input::KeyIdentifier::KI_F4;
    case KeyCode::KEY_F5:
        return Rocket::Core::Input::KeyIdentifier::KI_F5;
    case KeyCode::KEY_F6:
        return Rocket::Core::Input::KeyIdentifier::KI_F6;
    case KeyCode::KEY_F7:
        return Rocket::Core::Input::KeyIdentifier::KI_F7;
    case KeyCode::KEY_F8:
        return Rocket::Core::Input::KeyIdentifier::KI_F8;
    case KeyCode::KEY_F9:
        return Rocket::Core::Input::KeyIdentifier::KI_F9;
    case KeyCode::KEY_F10:
        return Rocket::Core::Input::KeyIdentifier::KI_F10;
    case KeyCode::KEY_F11:
        return Rocket::Core::Input::KeyIdentifier::KI_F11;
    case KeyCode::KEY_F12:
        return Rocket::Core::Input::KeyIdentifier::KI_F12;
    case KeyCode::KEY_0:
        return Rocket::Core::Input::KeyIdentifier::KI_0;
    case KeyCode::KEY_1:
        return Rocket::Core::Input::KeyIdentifier::KI_1;
    case KeyCode::KEY_2:
        return Rocket::Core::Input::KeyIdentifier::KI_2;
    case KeyCode::KEY_3:
        return Rocket::Core::Input::KeyIdentifier::KI_3;
    case KeyCode::KEY_4:
        return Rocket::Core::Input::KeyIdentifier::KI_4;
    case KeyCode::KEY_5:
        return Rocket::Core::Input::KeyIdentifier::KI_5;
    case KeyCode::KEY_6:
        return Rocket::Core::Input::KeyIdentifier::KI_6;
    case KeyCode::KEY_7:
        return Rocket::Core::Input::KeyIdentifier::KI_7;
    case KeyCode::KEY_8:
        return Rocket::Core::Input::KeyIdentifier::KI_8;
    case KeyCode::KEY_9:
        return Rocket::Core::Input::KeyIdentifier::KI_9;
    case KeyCode::KEY_A:
        return Rocket::Core::Input::KeyIdentifier::KI_A;
    case KeyCode::KEY_B:
        return Rocket::Core::Input::KeyIdentifier::KI_B;
    case KeyCode::KEY_C:
        return Rocket::Core::Input::KeyIdentifier::KI_C;
    case KeyCode::KEY_D:
        return Rocket::Core::Input::KeyIdentifier::KI_D;
    case KeyCode::KEY_E:
        return Rocket::Core::Input::KeyIdentifier::KI_E;
    case KeyCode::KEY_F:
        return Rocket::Core::Input::KeyIdentifier::KI_F;
    case KeyCode::KEY_G:
        return Rocket::Core::Input::KeyIdentifier::KI_G;
    case KeyCode::KEY_H:
        return Rocket::Core::Input::KeyIdentifier::KI_H;
    case KeyCode::KEY_I:
        return Rocket::Core::Input::KeyIdentifier::KI_I;
    case KeyCode::KEY_J:
        return Rocket::Core::Input::KeyIdentifier::KI_J;
    case KeyCode::KEY_K:
        return Rocket::Core::Input::KeyIdentifier::KI_K;
    case KeyCode::KEY_L:
        return Rocket::Core::Input::KeyIdentifier::KI_L;
    case KeyCode::KEY_M:
        return Rocket::Core::Input::KeyIdentifier::KI_M;
    case KeyCode::KEY_N:
        return Rocket::Core::Input::KeyIdentifier::KI_N;
    case KeyCode::KEY_O:
        return Rocket::Core::Input::KeyIdentifier::KI_O;
    case KeyCode::KEY_P:
        return Rocket::Core::Input::KeyIdentifier::KI_P;
    case KeyCode::KEY_Q:
        return Rocket::Core::Input::KeyIdentifier::KI_Q;
    case KeyCode::KEY_R:
        return Rocket::Core::Input::KeyIdentifier::KI_R;
    case KeyCode::KEY_S:
        return Rocket::Core::Input::KeyIdentifier::KI_S;
    case KeyCode::KEY_T:
        return Rocket::Core::Input::KeyIdentifier::KI_T;
    case KeyCode::KEY_U:
        return Rocket::Core::Input::KeyIdentifier::KI_U;
    case KeyCode::KEY_V:
        return Rocket::Core::Input::KeyIdentifier::KI_V;
    case KeyCode::KEY_W:
        return Rocket::Core::Input::KeyIdentifier::KI_W;
    case KeyCode::KEY_X:
        return Rocket::Core::Input::KeyIdentifier::KI_X;
    case KeyCode::KEY_Y:
        return Rocket::Core::Input::KeyIdentifier::KI_Y;
    case KeyCode::KEY_Z:
        return Rocket::Core::Input::KeyIdentifier::KI_Z;
    case KeyCode::KEY_LEFT_BRACKET:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_4;
    case KeyCode::KEY_RIGHT_BRACKET:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_6;
    case KeyCode::KEY_GRAVE_ACCENT:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_3;
    case KeyCode::KEY_SLASH:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_2;
    case KeyCode::KEY_BACKSLASH:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_102;
    case KeyCode::KEY_SPACE:
        return Rocket::Core::Input::KeyIdentifier::KI_SPACE;
    case KeyCode::KEY_EQUAL:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_PLUS;
    case KeyCode::KEY_MINUS:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_MINUS;
    case KeyCode::KEY_ESCAPE:
        return Rocket::Core::Input::KeyIdentifier::KI_ESCAPE;
    case KeyCode::KEY_ENTER:
        return Rocket::Core::Input::KeyIdentifier::KI_RETURN;
    case KeyCode::KEY_TAB:
        return Rocket::Core::Input::KeyIdentifier::KI_TAB;
    case KeyCode::KEY_BACKSPACE:
        return Rocket::Core::Input::KeyIdentifier::KI_BACK;
    case KeyCode::KEY_INSERT:
        return Rocket::Core::Input::KeyIdentifier::KI_INSERT;
    case KeyCode::KEY_DELETE:
        return Rocket::Core::Input::KeyIdentifier::KI_DELETE;
    case KeyCode::KEY_LEFT:
        return Rocket::Core::Input::KeyIdentifier::KI_LEFT;
    case KeyCode::KEY_RIGHT:
        return Rocket::Core::Input::KeyIdentifier::KI_RIGHT;
    case KeyCode::KEY_UP:
        return Rocket::Core::Input::KeyIdentifier::KI_UP;
    case KeyCode::KEY_DOWN:
        return Rocket::Core::Input::KeyIdentifier::KI_DOWN;
    case KeyCode::KEY_PAGE_UP:
        return Rocket::Core::Input::KeyIdentifier::KI_PRIOR;
    case KeyCode::KEY_PAGE_DOWN:
        return Rocket::Core::Input::KeyIdentifier::KI_NEXT;
    case KeyCode::KEY_HOME:
        return Rocket::Core::Input::KeyIdentifier::KI_HOME;
    case KeyCode::KEY_END:
        return Rocket::Core::Input::KeyIdentifier::KI_END;
    case KeyCode::KEY_CAPS_LOCK:
        return Rocket::Core::Input::KeyIdentifier::KI_CAPITAL;
    case KeyCode::KEY_SCROLL_LOCK:
        return Rocket::Core::Input::KeyIdentifier::KI_SCROLL;
    case KeyCode::KEY_NUM_LOCK:
        return Rocket::Core::Input::KeyIdentifier::KI_NUMLOCK;
    case KeyCode::KEY_PRINT_SCREEN:
        return Rocket::Core::Input::KeyIdentifier::KI_SNAPSHOT;
    case KeyCode::KEY_PAUSE:
        return Rocket::Core::Input::KeyIdentifier::KI_PAUSE;
    case KeyCode::KEY_LEFT_SHIFT:
        return Rocket::Core::Input::KeyIdentifier::KI_LSHIFT;
    case KeyCode::KEY_LEFT_ALT:
        return Rocket::Core::Input::KeyIdentifier::KI_LMENU;
    case KeyCode::KEY_LEFT_CTRL:
        return Rocket::Core::Input::KeyIdentifier::KI_LCONTROL;
    case KeyCode::KEY_RIGHT_SHIFT:
        return Rocket::Core::Input::KeyIdentifier::KI_RSHIFT;
    case KeyCode::KEY_RIGHT_ALT:
        return Rocket::Core::Input::KeyIdentifier::KI_RMENU;
    case KeyCode::KEY_RIGHT_CTRL:
        return Rocket::Core::Input::KeyIdentifier::KI_RCONTROL;
    case KeyCode::UNDEFINED:
    default:
        break;
    }

    return Rocket::Core::Input::KI_UNKNOWN;
}

Rocket::Core::Input::KeyModifier convertToRocketModifier(KeyModifier modifier)
{
    int res = 0;
    if (modifier & KM_SHIFT)
        res |= Rocket::Core::Input::KM_SHIFT;
    if (modifier & KM_ALT)
        res |= Rocket::Core::Input::KM_ALT;
    if (modifier & KM_CTRL)
        res |= Rocket::Core::Input::KM_CTRL;

    return static_cast<Rocket::Core::Input::KeyModifier>(res);
}

int convertToRocketMouseButtonIdx(MouseButton btn)
{
    switch (btn)
    {
    case MouseButton::LEFT:
        return 0;
    case MouseButton::RIGHT:
        return 1;
    case MouseButton::MIDDLE:
    case MouseButton::UNDEFINED:
    default:
        break;
    }

    return 2;
}

} }
