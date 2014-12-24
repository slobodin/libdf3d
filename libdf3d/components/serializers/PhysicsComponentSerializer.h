#pragma once

FWD_MODULE_CLASS(components, PhysicsComponent)

namespace df3d { namespace components { namespace serializers {

DF3D_DLL void load(components::PhysicsComponent *component, const char *definitionFile);
DF3D_DLL Json::Value save(const components::PhysicsComponent *component);

} } }