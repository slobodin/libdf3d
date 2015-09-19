#include "RocketKeyCodesAdapter.h"

namespace df3d {namespace gui {

Rocket::Core::Input::KeyIdentifier convertToRocketKeyCode(base::KeyboardEvent::KeyCode keycode)
{
    switch (keycode)
    {
    case base::KeyboardEvent::KEY_F1:
        return Rocket::Core::Input::KeyIdentifier::KI_F1;
    case base::KeyboardEvent::KEY_F2:
        return Rocket::Core::Input::KeyIdentifier::KI_F2;
    case base::KeyboardEvent::KEY_F3:
        return Rocket::Core::Input::KeyIdentifier::KI_F3;
    case base::KeyboardEvent::KEY_F4:
        return Rocket::Core::Input::KeyIdentifier::KI_F4;
    case base::KeyboardEvent::KEY_F5:
        return Rocket::Core::Input::KeyIdentifier::KI_F5;
    case base::KeyboardEvent::KEY_F6:
        return Rocket::Core::Input::KeyIdentifier::KI_F6;
    case base::KeyboardEvent::KEY_F7:
        return Rocket::Core::Input::KeyIdentifier::KI_F7;
    case base::KeyboardEvent::KEY_F8:
        return Rocket::Core::Input::KeyIdentifier::KI_F8;
    case base::KeyboardEvent::KEY_F9:
        return Rocket::Core::Input::KeyIdentifier::KI_F9;
    case base::KeyboardEvent::KEY_F10:
        return Rocket::Core::Input::KeyIdentifier::KI_F10;
    case base::KeyboardEvent::KEY_F11:
        return Rocket::Core::Input::KeyIdentifier::KI_F11;
    case base::KeyboardEvent::KEY_F12:
        return Rocket::Core::Input::KeyIdentifier::KI_F12;
    case base::KeyboardEvent::KEY_0:
        return Rocket::Core::Input::KeyIdentifier::KI_0;
    case base::KeyboardEvent::KEY_1:
        return Rocket::Core::Input::KeyIdentifier::KI_1;
    case base::KeyboardEvent::KEY_2:
        return Rocket::Core::Input::KeyIdentifier::KI_2;
    case base::KeyboardEvent::KEY_3:
        return Rocket::Core::Input::KeyIdentifier::KI_3;
    case base::KeyboardEvent::KEY_4:
        return Rocket::Core::Input::KeyIdentifier::KI_4;
    case base::KeyboardEvent::KEY_5:
        return Rocket::Core::Input::KeyIdentifier::KI_5;
    case base::KeyboardEvent::KEY_6:
        return Rocket::Core::Input::KeyIdentifier::KI_6;
    case base::KeyboardEvent::KEY_7:
        return Rocket::Core::Input::KeyIdentifier::KI_7;
    case base::KeyboardEvent::KEY_8:
        return Rocket::Core::Input::KeyIdentifier::KI_8;
    case base::KeyboardEvent::KEY_9:
        return Rocket::Core::Input::KeyIdentifier::KI_9;
    case base::KeyboardEvent::KEY_A:
        return Rocket::Core::Input::KeyIdentifier::KI_A;
    case base::KeyboardEvent::KEY_B:
        return Rocket::Core::Input::KeyIdentifier::KI_B;
    case base::KeyboardEvent::KEY_C:
        return Rocket::Core::Input::KeyIdentifier::KI_C;
    case base::KeyboardEvent::KEY_D:
        return Rocket::Core::Input::KeyIdentifier::KI_D;
    case base::KeyboardEvent::KEY_E:
        return Rocket::Core::Input::KeyIdentifier::KI_E;
    case base::KeyboardEvent::KEY_F:
        return Rocket::Core::Input::KeyIdentifier::KI_F;
    case base::KeyboardEvent::KEY_G:
        return Rocket::Core::Input::KeyIdentifier::KI_G;
    case base::KeyboardEvent::KEY_H:
        return Rocket::Core::Input::KeyIdentifier::KI_H;
    case base::KeyboardEvent::KEY_I:
        return Rocket::Core::Input::KeyIdentifier::KI_I;
    case base::KeyboardEvent::KEY_J:
        return Rocket::Core::Input::KeyIdentifier::KI_J;
    case base::KeyboardEvent::KEY_K:
        return Rocket::Core::Input::KeyIdentifier::KI_K;
    case base::KeyboardEvent::KEY_L:
        return Rocket::Core::Input::KeyIdentifier::KI_L;
    case base::KeyboardEvent::KEY_M:
        return Rocket::Core::Input::KeyIdentifier::KI_M;
    case base::KeyboardEvent::KEY_N:
        return Rocket::Core::Input::KeyIdentifier::KI_N;
    case base::KeyboardEvent::KEY_O:
        return Rocket::Core::Input::KeyIdentifier::KI_O;
    case base::KeyboardEvent::KEY_P:
        return Rocket::Core::Input::KeyIdentifier::KI_P;
    case base::KeyboardEvent::KEY_Q:
        return Rocket::Core::Input::KeyIdentifier::KI_Q;
    case base::KeyboardEvent::KEY_R:
        return Rocket::Core::Input::KeyIdentifier::KI_R;
    case base::KeyboardEvent::KEY_S:
        return Rocket::Core::Input::KeyIdentifier::KI_S;
    case base::KeyboardEvent::KEY_T:
        return Rocket::Core::Input::KeyIdentifier::KI_T;
    case base::KeyboardEvent::KEY_U:
        return Rocket::Core::Input::KeyIdentifier::KI_U;
    case base::KeyboardEvent::KEY_V:
        return Rocket::Core::Input::KeyIdentifier::KI_V;
    case base::KeyboardEvent::KEY_W:
        return Rocket::Core::Input::KeyIdentifier::KI_W;
    case base::KeyboardEvent::KEY_X:
        return Rocket::Core::Input::KeyIdentifier::KI_X;
    case base::KeyboardEvent::KEY_Y:
        return Rocket::Core::Input::KeyIdentifier::KI_Y;
    case base::KeyboardEvent::KEY_Z:
        return Rocket::Core::Input::KeyIdentifier::KI_Z;
    case base::KeyboardEvent::KEY_LEFT_BRACKET:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_4;
    case base::KeyboardEvent::KEY_RIGHT_BRACKET:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_6;
    case base::KeyboardEvent::KEY_GRAVE_ACCENT:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_3;
    case base::KeyboardEvent::KEY_SLASH:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_2;
    case base::KeyboardEvent::KEY_BACKSLASH:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_102;
    case base::KeyboardEvent::KEY_SPACE:
        return Rocket::Core::Input::KeyIdentifier::KI_SPACE;
    case base::KeyboardEvent::KEY_EQUAL:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_PLUS;
    case base::KeyboardEvent::KEY_MINUS:
        return Rocket::Core::Input::KeyIdentifier::KI_OEM_MINUS;
    case base::KeyboardEvent::KEY_ESCAPE:
        return Rocket::Core::Input::KeyIdentifier::KI_ESCAPE;
    case base::KeyboardEvent::KEY_ENTER:
        return Rocket::Core::Input::KeyIdentifier::KI_RETURN;
    case base::KeyboardEvent::KEY_TAB:
        return Rocket::Core::Input::KeyIdentifier::KI_TAB;
    case base::KeyboardEvent::KEY_BACKSPACE:
        return Rocket::Core::Input::KeyIdentifier::KI_BACK;
    case base::KeyboardEvent::KEY_INSERT:
        return Rocket::Core::Input::KeyIdentifier::KI_INSERT;
    case base::KeyboardEvent::KEY_DELETE:
        return Rocket::Core::Input::KeyIdentifier::KI_DELETE;
    case base::KeyboardEvent::KEY_LEFT:
        return Rocket::Core::Input::KeyIdentifier::KI_LEFT;
    case base::KeyboardEvent::KEY_RIGHT:
        return Rocket::Core::Input::KeyIdentifier::KI_RIGHT;
    case base::KeyboardEvent::KEY_UP:
        return Rocket::Core::Input::KeyIdentifier::KI_UP;
    case base::KeyboardEvent::KEY_DOWN:
        return Rocket::Core::Input::KeyIdentifier::KI_DOWN;
    case base::KeyboardEvent::KEY_PAGE_UP:
        return Rocket::Core::Input::KeyIdentifier::KI_PRIOR;
    case base::KeyboardEvent::KEY_PAGE_DOWN:
        return Rocket::Core::Input::KeyIdentifier::KI_NEXT;
    case base::KeyboardEvent::KEY_HOME:
        return Rocket::Core::Input::KeyIdentifier::KI_HOME;
    case base::KeyboardEvent::KEY_END:
        return Rocket::Core::Input::KeyIdentifier::KI_END;
    case base::KeyboardEvent::KEY_CAPS_LOCK:
        return Rocket::Core::Input::KeyIdentifier::KI_CAPITAL;
    case base::KeyboardEvent::KEY_SCROLL_LOCK:
        return Rocket::Core::Input::KeyIdentifier::KI_SCROLL;
    case base::KeyboardEvent::KEY_NUM_LOCK:
        return Rocket::Core::Input::KeyIdentifier::KI_NUMLOCK;
    case base::KeyboardEvent::KEY_PRINT_SCREEN:
        return Rocket::Core::Input::KeyIdentifier::KI_SNAPSHOT;
    case base::KeyboardEvent::KEY_PAUSE:
        return Rocket::Core::Input::KeyIdentifier::KI_PAUSE;
    case base::KeyboardEvent::KEY_LEFT_SHIFT:
        return Rocket::Core::Input::KeyIdentifier::KI_LSHIFT;
    case base::KeyboardEvent::KEY_LEFT_ALT:
        return Rocket::Core::Input::KeyIdentifier::KI_LMENU;
    case base::KeyboardEvent::KEY_LEFT_CTRL:
        return Rocket::Core::Input::KeyIdentifier::KI_LCONTROL;
    case base::KeyboardEvent::KEY_RIGHT_SHIFT:
        return Rocket::Core::Input::KeyIdentifier::KI_RSHIFT;
    case base::KeyboardEvent::KEY_RIGHT_ALT:
        return Rocket::Core::Input::KeyIdentifier::KI_RMENU;
    case base::KeyboardEvent::KEY_RIGHT_CTRL:
        return Rocket::Core::Input::KeyIdentifier::KI_RCONTROL;
    case base::KeyboardEvent::UNDEFINED:
    default:
        break;
    }

    return Rocket::Core::Input::KI_UNKNOWN;
}

Rocket::Core::Input::KeyModifier convertToRocketModifier(base::KeyboardEvent::KeyModifier modifier)
{
    int res = 0;
    if (modifier & base::KeyboardEvent::KM_SHIFT)
        res |= Rocket::Core::Input::KM_SHIFT;
    if (modifier & base::KeyboardEvent::KM_ALT)
        res |= Rocket::Core::Input::KM_ALT;
    if (modifier & base::KeyboardEvent::KM_CTRL)
        res |= Rocket::Core::Input::KM_CTRL;

    return static_cast<Rocket::Core::Input::KeyModifier>(res);
}

} }
