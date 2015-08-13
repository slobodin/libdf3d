#pragma once

#include <base/InputEvents.h>
#include <Rocket/Core/Input.h>

namespace df3d { namespace gui {

Rocket::Core::Input::KeyIdentifier convertToRocketKeyCode(base::KeyboardEvent::KeyCode keycode);

} }
