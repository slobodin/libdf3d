#include "RocketKeyCodesAdapter.h"

namespace df3d { namespace gui_impl {

Rocket::Core::Input::KeyIdentifier convertToRocketKeyCode(KeyboardEvent::KeyCode keycode)
{
    switch (keycode)
    {
    case KeyboardEvent::KEY_F1:
        return Rocket::Core::Input::KeyIdentifier::KI_F1;
    case KeyboardEvent::KEY_F2:
        return Rocket::Core::Input::KeyIdentifier::KI_F2;
    case KeyboardEvent::KEY_F3:
        return Rocket::Core::Input::KeyIdentifier::KI_F3;
    case KeyboardEvent::KEY_F4:
        return Rocket::Core::Input::KeyIdentifier::KI_F4;
    case KeyboardEvent::KEY_F5:
        return Rocket::Core::Input::KeyIdentifier::KI_F5;
    case KeyboardEvent::KEY_F6:
        return Rocket::Core::Input::KeyIdentifier::KI_F6;
    case KeyboardEvent::KEY_F7:
        return Rocket::Core::Input::KeyIdentifier::KI_F7;
    case KeyboardEvent::KEY_F8:
        return Rocket::Core::Input::KeyIdentifier::KI_F8;
    case KeyboardEvent::KEY_F9:
        return Rocket::Core::Input::KeyIdentifier::KI_F9;
    case KeyboardEvent::KEY_F10:
        return Rocket::Core::Input::KeyIdentifier::KI_F10;
    case KeyboardEvent::KEY_F11:
        return Rocket::Core::Input::KeyIdentifier::KI_F11;
    case KeyboardEvent::KEY_F12:
        return Rocket::Core::Input::KeyIdentifier::KI_F12;
    case KeyboardEvent::KEY_0:
        return Rocket::Core::Input::KeyIdentifier::KI_0;
    case KeyboardEvent::KEY_1:
        return Rocket::Core::Input::KeyIdentifier::KI_1;
    case KeyboardEvent::KEY_2:
        return Rocket::Core::Input::KeyIdentifier::KI_2;
    case KeyboardEvent::KEY_3:
        return Rocket::Core::Input::KeyIdentifier::KI_3;
    case KeyboardEvent::KEY_4:
        return Rocket::Core::Input::KeyIdentifier::KI_4;
    case KeyboardEvent::KEY_5:
        return Rocket::Core::Input::KeyIdentifier::KI_5;
    case KeyboardEvent::KEY_6:
        return Rocket::Core::Input::KeyIdentifier::KI_6;
    case KeyboardEvent::KEY_7:
        return Rocket::Core::Input::KeyIdentifier::KI_7;
    case KeyboardEvent::KEY_8:
        return Rocket::Core::Input::KeyIdentifier::KI_8;
    case KeyboardEvent::KEY_9:
        return Rocket::Core::Input::KeyIdentifier::KI_9;
    case KeyboardEvent::KEY_A:
        return Rocket::Core::Input::KeyIdentifier::KI_A;
    case KeyboardEvent::KEY_B:
        return Rocket::Core::Input::KeyIdentifier::KI_B;
    case KeyboardEvent::KEY_C:
        return Rocket::Core::Input::KeyIdentifier::KI_C;
    case KeyboardEvent::KEY_D:
        return Rocket::Core::Input::KeyIdentifier::KI_D;
    case KeyboardEvent::KEY_E:
        return Rocket::Core::Input::KeyIdentifier::KI_E;
    case KeyboardEvent::KEY_F:
        return Rocket::Core::Input::KeyIdentifier::KI_F;
    case KeyboardEvent::KEY_G:
        return Rocket::Core::Input::KeyIdentifier::KI_G;
    case KeyboardEvent::KEY_H:
        return Rocket::Core::Input::KeyIdentifier::KI_H;
    case KeyboardEvent::KEY_I:
        return Rocket::Core::Input::KeyIdentifier::KI_I;
    case KeyboardEvent::KEY_J:
        return Rocket::Core::Input::KeyIdentifier::KI_J;
    case KeyboardEvent::KEY_K:
        return Rocket::Core::Input::KeyIdentifier::KI_K;
    case KeyboardEvent::KEY_L:
        return Rocket::Core::Input::KeyIdentifier::KI_L;
    case KeyboardEvent::KEY_M:
        return Rocket::Core::Input::KeyIdentifier::KI_M;
    case KeyboardEvent::KEY_N:
        return Rocket::Core::Input::KeyIdentifier::KI_N;
    case KeyboardEvent::KEY_O:
        return Rocket::Core::Input::KeyIdentifier::KI_O;
    case KeyboardEvent::KEY_P:
        return Rocket::Core::Input::KeyIdentifier::KI_P;
    case KeyboardEvent::KEY_Q:
        return Rocket::Core::Input::KeyIdentifier::KI_Q;
    case KeyboardEvent::KEY_R:
        return Rocket::Core::Input::KeyIdentifier::KI_R;
    case KeyboardEvent::KEY_S:
        return Rocket::Core::Input::KeyIdentifier::KI_S;
    case KeyboardEvent::KEY_T:
        return Rocket::Core::Input::KeyIdentifier::KI_T;
    case KeyboardEvent::KEY_U:
        return Rocket::Core::Input::KeyIdentifier::KI_U;
    case KeyboardEvent::KEY_V:
        return Rocket::Core::Input::KeyIdentifier::KI_V;
    case KeyboardEvent::KEY_W:
        return Rocket::Core::Input::KeyIdentifier::KI_W;
    case KeyboardEvent::KEY_X:
        return Rocket::Core::Input::KeyIdentifier::KI_X;
    case KeyboardEvent::KEY_Y:
        return Rocket::Core::Input::KeyIdentifier::KI_Y;
    case KeyboardEvent::KEY_Z:
        return Rocket::Core::Input::KeyIdentifier::KI_Z;
    case KeyboardEvent::KEY_LEFT_BRACKET:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_4;
    case KeyboardEvent::KEY_RIGHT_BRACKET:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_6;
    case KeyboardEvent::KEY_GRAVE_ACCENT:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_3;
    case KeyboardEvent::KEY_SLASH:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_2;
    case KeyboardEvent::KEY_BACKSLASH:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_102;
    case KeyboardEvent::KEY_SPACE:
        return Rocket::Core::Input::KeyIdentifier::KI_SPACE;
    case KeyboardEvent::KEY_EQUAL:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_PLUS;
    case KeyboardEvent::KEY_MINUS:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_MINUS;
    case KeyboardEvent::KEY_ESCAPE:
        return Rocket::Core::Input::KeyIdentifier::KI_ESCAPE;
    case KeyboardEvent::KEY_ENTER:
        return Rocket::Core::Input::KeyIdentifier::KI_RETURN;
    case KeyboardEvent::KEY_TAB:
        return Rocket::Core::Input::KeyIdentifier::KI_TAB;
    case KeyboardEvent::KEY_BACKSPACE:
        return Rocket::Core::Input::KeyIdentifier::KI_BACK;
    case KeyboardEvent::KEY_INSERT:
        return Rocket::Core::Input::KeyIdentifier::KI_INSERT;
    case KeyboardEvent::KEY_DELETE:
        return Rocket::Core::Input::KeyIdentifier::KI_DELETE;
    case KeyboardEvent::KEY_LEFT:
        return Rocket::Core::Input::KeyIdentifier::KI_LEFT;
    case KeyboardEvent::KEY_RIGHT:
        return Rocket::Core::Input::KeyIdentifier::KI_RIGHT;
    case KeyboardEvent::KEY_UP:
        return Rocket::Core::Input::KeyIdentifier::KI_UP;
    case KeyboardEvent::KEY_DOWN:
        return Rocket::Core::Input::KeyIdentifier::KI_DOWN;
    case KeyboardEvent::KEY_PAGE_UP:
        return Rocket::Core::Input::KeyIdentifier::KI_PRIOR;
    case KeyboardEvent::KEY_PAGE_DOWN:
        return Rocket::Core::Input::KeyIdentifier::KI_NEXT;
    case KeyboardEvent::KEY_HOME:
        return Rocket::Core::Input::KeyIdentifier::KI_HOME;
    case KeyboardEvent::KEY_END:
        return Rocket::Core::Input::KeyIdentifier::KI_END;
    case KeyboardEvent::KEY_CAPS_LOCK:
        return Rocket::Core::Input::KeyIdentifier::KI_CAPITAL;
    case KeyboardEvent::KEY_SCROLL_LOCK:
        return Rocket::Core::Input::KeyIdentifier::KI_SCROLL;
    case KeyboardEvent::KEY_NUM_LOCK:
        return Rocket::Core::Input::KeyIdentifier::KI_NUMLOCK;
    case KeyboardEvent::KEY_PRINT_SCREEN:
        return Rocket::Core::Input::KeyIdentifier::KI_SNAPSHOT;
    case KeyboardEvent::KEY_PAUSE:
        return Rocket::Core::Input::KeyIdentifier::KI_PAUSE;
    case KeyboardEvent::KEY_LEFT_SHIFT:
        return Rocket::Core::Input::KeyIdentifier::KI_LSHIFT;
    case KeyboardEvent::KEY_LEFT_ALT:
        return Rocket::Core::Input::KeyIdentifier::KI_LMENU;
    case KeyboardEvent::KEY_LEFT_CTRL:
        return Rocket::Core::Input::KeyIdentifier::KI_LCONTROL;
    case KeyboardEvent::KEY_RIGHT_SHIFT:
        return Rocket::Core::Input::KeyIdentifier::KI_RSHIFT;
    case KeyboardEvent::KEY_RIGHT_ALT:
        return Rocket::Core::Input::KeyIdentifier::KI_RMENU;
    case KeyboardEvent::KEY_RIGHT_CTRL:
        return Rocket::Core::Input::KeyIdentifier::KI_RCONTROL;
    case KeyboardEvent::UNDEFINED:
    default:
        break;
    }

    return Rocket::Core::Input::KI_UNKNOWN;
}

Rocket::Core::Input::KeyModifier convertToRocketModifier(KeyboardEvent::KeyModifier modifier)
{
    int res = 0;
    if (modifier & KeyboardEvent::KM_SHIFT)
        res |= Rocket::Core::Input::KM_SHIFT;
    if (modifier & KeyboardEvent::KM_ALT)
        res |= Rocket::Core::Input::KM_ALT;
    if (modifier & KeyboardEvent::KM_CTRL)
        res |= Rocket::Core::Input::KM_CTRL;

    return static_cast<Rocket::Core::Input::KeyModifier>(res);
}

} }
