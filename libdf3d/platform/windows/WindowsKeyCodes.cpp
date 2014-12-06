#include "df3d_pch.h"
#include "WindowsKeyCodes.h"

namespace df3d { namespace platform {

base::KeyboardEvent::KeyCode convertKeyCode(WPARAM keycode)
{
    switch (keycode)
    {
    case VK_UP:
        return base::KeyboardEvent::KeyCode::UP;
    case VK_DOWN:
        return base::KeyboardEvent::KeyCode::DOWN;
    case VK_LEFT:
        return base::KeyboardEvent::KeyCode::LEFT;
    case VK_RIGHT:
        return base::KeyboardEvent::KeyCode::RIGHT;
    default:
        break;
    }

    return base::KeyboardEvent::KeyCode::UNDEFINED;
}

} }