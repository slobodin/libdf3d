#include "stdafx.h"

void bindSdl()
{
    using namespace boost::python;

    // SDL key codes.
    scope().attr("SDLK_UNKNOWN") = (SDL_Keycode)SDLK_UNKNOWN;
    scope().attr("SDLK_a") = (SDL_Keycode)SDLK_a;
    scope().attr("SDLK_b") = (SDL_Keycode)SDLK_b;
    scope().attr("SDLK_c") = (SDL_Keycode)SDLK_c;
    scope().attr("SDLK_d") = (SDL_Keycode)SDLK_d;
    scope().attr("SDLK_e") = (SDL_Keycode)SDLK_e;
    scope().attr("SDLK_f") = (SDL_Keycode)SDLK_f;
    scope().attr("SDLK_g") = (SDL_Keycode)SDLK_g;
    scope().attr("SDLK_h") = (SDL_Keycode)SDLK_h;
    scope().attr("SDLK_i") = (SDL_Keycode)SDLK_i;
    scope().attr("SDLK_j") = (SDL_Keycode)SDLK_j;
    scope().attr("SDLK_k") = (SDL_Keycode)SDLK_k;
    scope().attr("SDLK_l") = (SDL_Keycode)SDLK_l;
    scope().attr("SDLK_m") = (SDL_Keycode)SDLK_m;
    scope().attr("SDLK_n") = (SDL_Keycode)SDLK_n;
    scope().attr("SDLK_o") = (SDL_Keycode)SDLK_o;
    scope().attr("SDLK_p") = (SDL_Keycode)SDLK_p;
    scope().attr("SDLK_q") = (SDL_Keycode)SDLK_q;
    scope().attr("SDLK_r") = (SDL_Keycode)SDLK_r;
    scope().attr("SDLK_s") = (SDL_Keycode)SDLK_s;
    scope().attr("SDLK_t") = (SDL_Keycode)SDLK_t;
    scope().attr("SDLK_u") = (SDL_Keycode)SDLK_u;
    scope().attr("SDLK_v") = (SDL_Keycode)SDLK_v;
    scope().attr("SDLK_w") = (SDL_Keycode)SDLK_w;
    scope().attr("SDLK_x") = (SDL_Keycode)SDLK_x;
    scope().attr("SDLK_y") = (SDL_Keycode)SDLK_y;
    scope().attr("SDLK_z") = (SDL_Keycode)SDLK_z;
    scope().attr("SDLK_RETURN") = (SDL_Keycode)SDLK_RETURN;
    scope().attr("SDLK_ESCAPE") = (SDL_Keycode)SDLK_ESCAPE;
    scope().attr("SDLK_BACKSPACE") = (SDL_Keycode)SDLK_BACKSPACE;
    scope().attr("SDLK_TAB") = (SDL_Keycode)SDLK_TAB;
    scope().attr("SDLK_SPACE") = (SDL_Keycode)SDLK_SPACE;
    scope().attr("SDLK_RIGHT") = (SDL_Keycode)SDLK_RIGHT;
    scope().attr("SDLK_LEFT") = (SDL_Keycode)SDLK_LEFT;
    scope().attr("SDLK_DOWN") = (SDL_Keycode)SDLK_DOWN;
    scope().attr("SDLK_UP") = (SDL_Keycode)SDLK_UP;

    // Mouse events.
    scope().attr("SDL_BUTTON_LMASK") = SDL_BUTTON_LMASK;
    scope().attr("SDL_BUTTON_RMASK") = SDL_BUTTON_RMASK;

    class_<SDL_MouseMotionEvent>("SDL_MouseMotionEvent")
        .def_readonly("state", &SDL_MouseMotionEvent::state)
        .def_readonly("x", &SDL_MouseMotionEvent::x)
        .def_readonly("y", &SDL_MouseMotionEvent::y)
    ;

    enum_<SDL_EventType>("SDL_EventType")
        .value("SDL_QUIT", SDL_QUIT)
        .value("SDL_APP_TERMINATING", SDL_APP_TERMINATING)
        .value("SDL_APP_LOWMEMORY", SDL_APP_LOWMEMORY)
        .value("SDL_APP_WILLENTERBACKGROUND", SDL_APP_WILLENTERBACKGROUND)
        .value("SDL_APP_DIDENTERBACKGROUND", SDL_APP_DIDENTERBACKGROUND)
        .value("SDL_APP_WILLENTERFOREGROUND", SDL_APP_WILLENTERFOREGROUND)
        .value("SDL_APP_DIDENTERFOREGROUND", SDL_APP_DIDENTERFOREGROUND)
        .value("SDL_WINDOWEVENT", SDL_WINDOWEVENT)
        .value("SDL_SYSWMEVENT", SDL_SYSWMEVENT)
        .value("SDL_KEYDOWN", SDL_KEYDOWN)
        .value("SDL_KEYUP", SDL_KEYUP)
        .value("SDL_TEXTEDITING", SDL_TEXTEDITING)
        .value("SDL_TEXTINPUT", SDL_TEXTINPUT)
        .value("SDL_MOUSEMOTION", SDL_MOUSEMOTION)
        .value("SDL_MOUSEBUTTONDOWN", SDL_MOUSEBUTTONDOWN)
        .value("SDL_MOUSEBUTTONUP", SDL_MOUSEBUTTONUP)
        .value("SDL_MOUSEWHEEL", SDL_MOUSEWHEEL)
        .value("SDL_FINGERDOWN", SDL_FINGERDOWN)
        .value("SDL_FINGERUP", SDL_FINGERUP)
        .value("SDL_FINGERMOTION", SDL_FINGERMOTION)
        .value("SDL_DROPFILE", SDL_DROPFILE)
    ;

    scope().attr("SDL_PRESSED") = SDL_PRESSED;
    scope().attr("SDL_RELEASED") = SDL_RELEASED;

    scope().attr("SDL_BUTTON_LEFT") = SDL_BUTTON_LEFT;
    scope().attr("SDL_BUTTON_MIDDLE") = SDL_BUTTON_MIDDLE;
    scope().attr("SDL_BUTTON_RIGHT") = SDL_BUTTON_RIGHT;
}