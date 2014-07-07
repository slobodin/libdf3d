#include "df3d_pch.h"
#include "RocketKeyBindings.h"

namespace df3d { namespace gui {

Rocket::Core::Input::KeyIdentifier RocketKeycodeFromSdl(SDL_Keycode keyId)
{
    switch (keyId)
    {
    case SDLK_RETURN:
        return Rocket::Core::Input::KI_RETURN;
    case SDLK_SPACE:
        return Rocket::Core::Input::KI_SPACE;
    case SDLK_ESCAPE:
        return Rocket::Core::Input::KI_ESCAPE;
    case SDLK_BACKSPACE:
        return Rocket::Core::Input::KI_BACK;
    case SDLK_TAB:
        return Rocket::Core::Input::KI_TAB;
    case SDLK_PLUS:
        return Rocket::Core::Input::KI_OEM_PLUS;
    case SDLK_COMMA:
        return Rocket::Core::Input::KI_OEM_COMMA;
    case SDLK_MINUS:
        return Rocket::Core::Input::KI_OEM_MINUS;
    case SDLK_PERIOD:
        return Rocket::Core::Input::KI_OEM_PERIOD;
    case SDLK_DELETE:
        return Rocket::Core::Input::KI_DELETE;
    case SDLK_0:
        return Rocket::Core::Input::KI_0;
    case SDLK_1:
        return Rocket::Core::Input::KI_1;
    case SDLK_2:
        return Rocket::Core::Input::KI_2;
    case SDLK_3:
        return Rocket::Core::Input::KI_3;
    case SDLK_4:
        return Rocket::Core::Input::KI_4;
    case SDLK_5:
        return Rocket::Core::Input::KI_5;
    case SDLK_6:
        return Rocket::Core::Input::KI_6;
    case SDLK_7:
        return Rocket::Core::Input::KI_7;
    case SDLK_8:
        return Rocket::Core::Input::KI_8;
    case SDLK_9:
        return Rocket::Core::Input::KI_9;
    case SDLK_a:
        return Rocket::Core::Input::KI_A;
    case SDLK_b:
        return Rocket::Core::Input::KI_B;
    case SDLK_c:
        return Rocket::Core::Input::KI_C;
    case SDLK_d:
        return Rocket::Core::Input::KI_D;
    case SDLK_e:
        return Rocket::Core::Input::KI_E;
    case SDLK_f:
        return Rocket::Core::Input::KI_F;
    case SDLK_g:
        return Rocket::Core::Input::KI_G;
    case SDLK_h:
        return Rocket::Core::Input::KI_H;
    case SDLK_i:
        return Rocket::Core::Input::KI_I;
    case SDLK_j:
        return Rocket::Core::Input::KI_J;
    case SDLK_k:
        return Rocket::Core::Input::KI_K;
    case SDLK_l:
        return Rocket::Core::Input::KI_L;
    case SDLK_m:
        return Rocket::Core::Input::KI_M;
    case SDLK_n:
        return Rocket::Core::Input::KI_N;
    case SDLK_o:
        return Rocket::Core::Input::KI_O;
    case SDLK_p:
        return Rocket::Core::Input::KI_P;
    case SDLK_q:
        return Rocket::Core::Input::KI_Q;
    case SDLK_r:
        return Rocket::Core::Input::KI_R;
    case SDLK_s:
        return Rocket::Core::Input::KI_S;
    case SDLK_t:
        return Rocket::Core::Input::KI_T;
    case SDLK_u:
        return Rocket::Core::Input::KI_U;
    case SDLK_v:
        return Rocket::Core::Input::KI_V;
    case SDLK_w:
        return Rocket::Core::Input::KI_W;
    case SDLK_x:
        return Rocket::Core::Input::KI_X;
    case SDLK_y:
        return Rocket::Core::Input::KI_Y;
    case SDLK_z:
        return Rocket::Core::Input::KI_Z;

    default:
        break;
    }

    return Rocket::Core::Input::KI_UNKNOWN;
}

int RocketKeyModifierFromSdl(SDL_Keysym keysym)
{
    int keyState = 0;
    if (keysym.mod & KMOD_SHIFT)
        keyState |= Rocket::Core::Input::KM_SHIFT;
    if (keysym.mod & KMOD_CTRL)
        keyState |= Rocket::Core::Input::KM_CTRL;
    if (keysym.mod & KMOD_ALT)
        keyState |= Rocket::Core::Input::KM_ALT;
    if (keysym.mod & KMOD_CAPS)
        keyState |= Rocket::Core::Input::KM_CAPSLOCK;

    return keyState;
}



} }