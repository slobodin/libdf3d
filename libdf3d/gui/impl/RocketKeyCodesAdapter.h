#pragma once

#include <input/InputEvents.h>
#include <Rocket/Core/Input.h>

namespace df3d {

Rocket::Core::Input::KeyIdentifier convertToRocketKeyCode(KeyboardEvent::KeyCode keycode);
Rocket::Core::Input::KeyModifier convertToRocketModifier(KeyboardEvent::KeyModifier modifier);

}
