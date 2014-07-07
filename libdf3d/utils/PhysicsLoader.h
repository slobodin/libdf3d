#pragma once

FWD_MODULE_CLASS(components, PhysicsComponent)

namespace df3d { namespace utils { namespace physics_loader {

void init(components::PhysicsComponent *component, const char *definitionFile);

} } }