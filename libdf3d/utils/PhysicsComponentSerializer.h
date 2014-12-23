#pragma once

FWD_MODULE_CLASS(components, PhysicsComponent)

namespace df3d { namespace utils { namespace serializers {

DF3D_DLL void load(components::PhysicsComponent *component, const char *definitionFile);
DF3D_DLL void save(const components::PhysicsComponent *component, const char *definitionFile);

} } }