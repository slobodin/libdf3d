#include "df3d_pch.h"
#include "InputEvents.h"

namespace df3d { namespace base {

MouseMotionEvent::MouseMotionEvent(const SDL_MouseMotionEvent &other)
    : x(other.x), 
    y(other.y), 
    leftPressed(other.state & SDL_BUTTON_LMASK),
    rightPressed(other.state & SDL_BUTTON_RMASK)
{
}

} }