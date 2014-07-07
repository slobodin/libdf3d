#pragma once

#include <Rocket/Core/Input.h>

namespace df3d { namespace gui {

Rocket::Core::Input::KeyIdentifier RocketKeycodeFromSdl(SDL_Keycode keyId);
int RocketKeyModifierFromSdl(SDL_Keysym keysym);

} }