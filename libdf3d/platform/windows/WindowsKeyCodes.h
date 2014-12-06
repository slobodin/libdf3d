#pragma once

#include <base/InputEvents.h>

#include <Windows.h>

namespace df3d { namespace platform {

base::KeyboardEvent::KeyCode convertKeyCode(WPARAM keycode);

} }