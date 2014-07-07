#pragma once

struct SDL_MouseMotionEvent;

namespace df3d { namespace base {

class MouseMotionEvent
{
public:
    MouseMotionEvent() { }
    MouseMotionEvent(const SDL_MouseMotionEvent &other);

    bool leftPressed = false;
    bool rightPressed = false;
    int x = 0;
    int y = 0;
};

} }