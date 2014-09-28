#pragma once

namespace df3d { namespace base {

class DF3D_DLL MouseMotionEvent
{
public:
    bool leftPressed = false;
    bool rightPressed = false;
    int x = 0;
    int y = 0;

    MouseMotionEvent(const SDL_MouseMotionEvent &other);
};

} }