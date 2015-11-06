#pragma once

#include <base/InputEvents.h>
#include <Rocket/Core/Input.h>

namespace df3d { namespace gui {

Rocket::Core::Input::KeyIdentifier convertToRocketKeyCode(KeyboardEvent::KeyCode keycode);
Rocket::Core::Input::KeyModifier convertToRocketModifier(KeyboardEvent::KeyModifier modifier);

} }
