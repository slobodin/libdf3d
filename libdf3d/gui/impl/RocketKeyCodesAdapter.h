#pragma once

#include <input/InputEvents.h>
#include <Rocket/Core/Input.h>

namespace df3d { namespace gui_impl {

Rocket::Core::Input::KeyIdentifier convertToRocketKeyCode(KeyCode keycode);
Rocket::Core::Input::KeyModifier convertToRocketModifier(KeyModifier modifier);
int convertToRocketMouseButtonIdx(MouseButton btn);

} }
